#include "main.hpp"


/** ray_cast to find out if a ray intersects with a sphere
 *
 * @param start    starting position of ray
 * @param dir      direction line is pointing
 *                   should be a unit vector
 *
 * @param hit_loc
 * @param hit_idx
 * @return type
 */
HIT::type ray_cast(const v3d& start, const v3d& dir,
		v3d *hit_loc, unsigned int *hit_idx) {
#if DEBUG
	assert(IS_APPROX_0(dir.length() - 1, 1e-6));
#endif
	
	HIT::type hit = HIT::undef;

	
	{// spheres
		//timer time_spheres("ray spheres");

		unsigned int min_idx = UINT_MAX;
		double min_dist = DBL_MAX;
		for(unsigned int i = 0; i < num_spheres; i++) {
			/*
			 * from wikipedia:
			 *    https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
			 *
			 * c centre
			 * r radius
			 *
			 * d distance along line intersection has occurred
			 * l direction of line (unit vector)
			 * o origin of line
			 *
			 * [x] = len of x
			 * d = -(l(o-c)) +- sqrt((l(o-c))^2 - (([o-c]^2) - r^2))
			 *
			 * c_to_o = line from centre to origin
			 * l(o-c) = lc_to_o = dot(line, c_to_o)
			 */
			v3d c = spheres[i].c;
			double r = spheres[i].r;

			v3d o = start;
			v3d l = dir;
			double d;

			v3d c_to_o = o - c;
			double lc_to_o = l.dot(o-c);

			double discriminant = (lc_to_o*lc_to_o - ((c_to_o).length_squared() - r*r));

			// 1 or more sol'ns
			if(discriminant >= 0) {

				double sqrt_discrim = sqrt(discriminant);
				d = fmin(-lc_to_o + sqrt_discrim, -lc_to_o - sqrt_discrim);

				if(d < min_dist && d > 0) {
					hit = HIT::sphere;
					min_dist = d;
					min_idx = i;
				}
			}
		}

		if(hit == HIT::sphere) {
			if(hit_loc) *hit_loc = start + dir * min_dist;
			if(hit_idx) *hit_idx = min_idx;
		}
	}

	return hit;
}







std::string get_mouse_name(char *devices_list) {

	bool found_event_name = false;
	std::string event_name;

	do {
		FILE *stream;

		if(!(stream = fopen(devices_list, "r"))) {
			perror(devices_list);
			break;
		}
		std::unique_ptr<FILE, void(*)(void*)> holder(stream, (void(*)(void*))fclose);


		// regex to find the line that handles mouse input
		size_t nmatch_ms = 2;
		regmatch_t pmatch_ms[nmatch_ms];
		regex_t match_mouse;
		char *regx_ms = (char*)"Handlers=(.*mouse[0-9]+.*)";
		regcomp(&match_mouse, regx_ms, REG_EXTENDED);

		//regex to find the element within that line
		size_t nmatch_evt = 2;
		regmatch_t pmatch_evt[nmatch_evt];
		regex_t match_event;
		char *regx_evt = (char*)".*(event[0-9]+).*";
		regcomp(&match_event, regx_evt, REG_EXTENDED);

		{
			// we can't use a unique_ptr here because it is null,
			//  so the unique_ptr would not do its job
			char *line = NULL;
			ssize_t size_read;
			size_t size_of_buffer;
			while((size_read = getline(&line, &size_of_buffer, stream))) {

				if(regexec(&match_mouse, line, nmatch_ms, pmatch_ms, 0) != REG_NOMATCH) {
					char *mouse_section = line + pmatch_ms[1].rm_so;
					if(regexec(&match_event, mouse_section, nmatch_evt, pmatch_evt, 0) != REG_NOMATCH) {
						event_name.assign(
								mouse_section + pmatch_evt[1].rm_so,
								pmatch_evt[1].rm_eo - pmatch_evt[1].rm_so);

						// successfully found a thing
						found_event_name = true;
						break;// reason for 'do {} while(false);'
					}
				}

			}
			// getline mallocs it regardless of success, so we need
			//  to free it unconditionally
			free(line);
		}



	} while(false);

	if(found_event_name) {
		//printf("found evt name!: '%s'\n", event_name.c_str());
		return event_name;
	} else {
		//printf("no event found :(");
		return nullptr;
	}
}






void* keys(void *curse_shared_ptr) {
	std::shared_ptr<curse> curses_ptr = *(std::shared_ptr<curse>*)(curse_shared_ptr);

	//std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
	//std::chrono::duration<double> time_delta;

	std::chrono::time_point<std::chrono::steady_clock> current_time = std::chrono::steady_clock::now();
	quit_mutex.lock("enter keys loop");
	do {
	quit_mutex.unlock();
		//time_delta = std::chrono::steady_clock::now() - start_time;
		//mvprintw(0, 0, "time delta: %.0f", time_delta);


		std::chrono::time_point<std::chrono::steady_clock> last_time = current_time;
		current_time = std::chrono::steady_clock::now();


		int key = getch();

		// get the 3 unit vectors relative to direction
		direction_mutex.lock("keys copy dir");
		v3d forward = direction;
		direction_mutex.unlock();
		// set forward vector to be not looking up or down
		forward.y = 0; forward.normalise();
		v3d up = v3d::Y;
		v3d right = v3d::cross(forward, up);

		std::chrono::duration<double> delta;
		switch(key) {
			case KEY_UP:
			case (int)'w':
				delta = current_time - last_time;
				translation_mutex.lock("keys press w");
				translation += forward * units_per_sec * delta.count();
				translation_mutex.unlock();
				break;
			case KEY_DOWN:
			case (int)'s':
				delta = current_time - last_time;
				translation_mutex.lock("keys press s");
				translation -= forward * units_per_sec * delta.count();
				translation_mutex.unlock();
				break;
			case KEY_RIGHT:
			case (int)'d':
				delta = current_time - last_time;
				translation_mutex.lock("keys press d");
				translation += right * units_per_sec * delta.count();
				translation_mutex.unlock();
				break;
			case KEY_LEFT:
			case (int)'a':
				delta = current_time - last_time;
				translation_mutex.lock("keys press a");
				translation -= right * units_per_sec * delta.count();
				translation_mutex.unlock();
				break;

			case (int)' ':
				delta = current_time - last_time;
				translation_mutex.lock("keys press \" \"");
				translation += up * units_per_sec * delta.count();
				translation_mutex.unlock();
				break;
			case (int)'c':
				delta = current_time - last_time;
				translation_mutex.lock("keys press c");
				translation -= up * units_per_sec * delta.count();
				translation_mutex.unlock();
				break;

			case (int)'+':
			case (int)'='://no shift held
				fov_mutex.lock("keys press +");
				fov *= 1.5;
				fov_mutex.unlock();
				break;
			case (int)'-':
			case (int)'_'://shift held
				fov_mutex.lock("keys press -");
				fov /= 1.5;
				fov_mutex.unlock();
				break;



			case 'q':
			case KEY_EXIT:
				quit_mutex.lock("keys request quit");
				quit = true;
				quit_mutex.unlock();
				break;
		}

		quit_mutex.lock("keys re-loop");
	} while(!quit);
	quit_mutex.unlock();

	return NULL;
}







void* mouse(void *evt_name) {
	std::string event_name = *(std::string*)evt_name;

	int fd;
	struct input_event ie;

	size_t event_loc_len = 256;
	char event_loc[event_loc_len];
	snprintf(event_loc, event_loc_len, "/dev/input/%s", event_name.c_str());

	if((fd = open(event_loc, O_RDONLY)) == -1) {
		perror("opening device");
		printf("%s\n", event_loc);
		return nullptr;
	}

	double x_deg = 0;
	double y_deg = 0;

	while(read(fd, &ie, sizeof(struct input_event))) {
		//printf("time %ld.%06ld\ttype %d\tcode %d\tvalue %d\n",
		//    ie.time.tv_sec, ie.time.tv_usec, ie.type, ie.code, ie.value);

		quit_mutex.lock("mouse check for quit");
		if(quit) {
			quit_mutex.unlock();
			break;
		}
		quit_mutex.unlock();

		switch(ie.type) {
			case EV_SYN:
				break;

			case EV_KEY:
				switch(ie.code) {
					case BTN_LEFT:
						//printf("\tBTN_LEFT\t");
						break;
					case BTN_RIGHT:
						//printf("\tBTN_RIGHT\t");
						break;
					case BTN_MIDDLE:
						//printf("\tBTN_MIDDLE\t");
						break;
				}

				if(ie.value) {
					//printf("down\n");
				} else {
					//printf("up\n");
				} break;

			case EV_REL:
				//printf("relative: %3d\t", ie.value);
				switch(ie.code) {
					case REL_X:
						x_deg -= ie.value;
						while(x_deg > 360) {
							x_deg -= 360;
						}
						while(x_deg < 0) {
							x_deg += 360;
						}
						//x_deg %= 180;
						if(ie.value < 0) {
							//printf("left\n");
						} else {
							//printf("right\n");
						}
						break;
					case REL_Y:
						int temp = y_deg - ie.value;
						if(temp < -90) {
							temp = -90;
						} else if(temp > 90) {
							temp = 90;
						}
						y_deg = temp;

						if(ie.value < 0) {
							//printf("up\n");
						} else {
							//printf("down\n");
						}
						break;
				}
				break;
			case EV_ABS: break;
		}

		// set direction to be (1, 0, 0) rotated by x and y degrees
		direction_mutex.lock("mouse rotate dir");

		// rotate direction
		direction = 
			v3d::rotate(v3d::X, y_deg, -v3d::Z)
			.rotate(x_deg, -v3d::Y);

		// create a transformation matrix that will rotate any vector
		// the same way as rotating the above vector
#if 1
		rotate_tx = m44d::make_transformation([=](const v3d& to_rotate) {
				return v3d::rotate(to_rotate, y_deg, -v3d::Z)
				.rotate(x_deg, -v3d::Y);
				});
#else

		rotate_tx = m44d::make_transformation(
				v3d::rotate(v3d::X, y_deg, -v3d::Z).rotate(x_deg, -v3d::Y),
				v3d::rotate(v3d::Y, y_deg, -v3d::Z).rotate(x_deg, -v3d::Y),
				v3d::rotate(v3d::Z, y_deg, -v3d::Z).rotate(x_deg, -v3d::Y)
				);

#endif
#if 0
		std::ofstream rot_tx_file("rot_tx.csv", std::ios::out);
		rot_tx_file << std::endl << "rot_tx: " << rotate_tx << std::endl << std::endl;
#endif

		assert(!pthread_cond_signal(&main_start));

		direction_mutex.unlock();
	}

	return NULL;
}



template<typename T>
struct array_2d {
	const size_t M;
	public:
		std::unique_ptr<T[]> arr;

		constexpr array_2d(size_t n, size_t m):
			M(m),
			arr(std::make_unique<T[]>(n * m)) {};

		// allow arr operator for first idx
		T* operator[](size_t i) {
			return arr.get() + i*M;
		}

		// have the interface of a unique_ptr
		operator std::unique_ptr<T[]>() {
			return arr;
		}
};


#define USE_ARR_2D 1
#if USE_ARR_2D
/** makes a pixel array for a rectillinear projection
 *
 * creates an array of vectors of unit length, each correspoinding to
 * a pixel in a rectillinear projection.
 * the direction everything points to is (1,0,0)
 *
 * @param x      x resolution
 * @param y      y resolution
 * @param hfov   horizontal fov: must be <= 180
 *
 * @returns   a struct containing a unique_ptr,
 *            but can be indexed up to x and y to retrieve a pixel
 */
#if 0
array_2d<v3d> make_pixel_arr(size_t x, size_t y, double hfov) {
	assert(fov < 180);
	array_2d<v3d> out(x, y);

	/*
	 *             x
	 *     point -----
	 *           \   |
	 *            \  |1
	 *             \θ|
	 *              \|
	 *              start
	 *
	 * tan(θ) = o/a
	 * x = tan(fov/2)
	 */

	const double vfov = hfov * y / x;

	const double start_x = tan((PI/180) * hfov/2);
	const double start_y = tan((PI/180) * vfov/2);

	for(size_t i = 0; i < x; i++)
		for(size_t j = 0; j < y; j++) {
			out[i][j] = v3d::normalise(v3d(1,
						-start_y + 2*start_y*j,
						-start_x + 2*start_x*i));
		}

	return out;
}
#endif

array_2d<v3d> make_pixel_arr(size_t x, size_t y, double hfov) {
	const double vfov = hfov * y / x;
	array_2d<v3d> out(x, y);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
	// x gets converted to unsigned int, but all 
	for(size_t i = 0; i < x && i < INT_MAX; i++) {
#pragma GCC diagnostic pop

		// account for difference in angle of different pixels
		double xr_on_2 = x/2.0;
		double angle = hfov * (i - xr_on_2) / (double)xr_on_2;
		v3d pix_dir_x = v3d::rotate(v3d::X, angle, v3d::Y);
		v3d new_right = v3d::rotate(v3d::Z, angle, v3d::Y);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
		for(size_t j = 0; j < y && j < INT_MAX; j++) {
#pragma GCC diagnostic pop

			// angle of pixels, but for i axis
			double yr_on_2 = y/2.0;
			double angle = vfov * (j - yr_on_2) / (double)yr_on_2;
			out[i][j] = v3d::rotate(pix_dir_x, angle, new_right);
		}
	}
	return out;
}
#else

// temp version just using malloc for simplicity
v3d* make_pixel_arr(size_t x, size_t y, double hfov) {
	assert(fov < 180);
	v3d *out = reinterpret_cast<v3d*>(malloc(sizeof(v3d) * x * y));

	/*
	 *             x
	 *     point -----
	 *           \   |
	 *            \  |1
	 *             \θ|
	 *              \|
	 *              start
	 *
	 * tan(θ) = o/a
	 * x = tan(fov/2)
	 */

	const double vfov = hfov * y / x;

	const double start_x = tan((PI/180) * hfov/2);
	const double start_y = tan((PI/180) * vfov/2);

	for(size_t i = 0; i < x; i++)
		for(size_t j = 0; j < y; j++) {
			out[i+x*j] = v3d::normalise(v3d(1,
						start_y + (2*start_y*j)/y,
						start_x + (2*start_x*i)/x));
		}

	return out;
}
#endif


int main(int argc, char **argv) {
	timer main_timer("main timer");


	bool one_render = false;
	bool scripted_movement = false;

	// do args
	if(argc > 1) {
		int num_args = 0;
		for(int i = 1; i < argc; i++) {
			if(strncmp(argv[i], arg_onerender, sizeof(arg_onerender)) == 0) {
				printf("doing one render\n");
				one_render = true;
				num_args++;
			} else if(strncmp(argv[i], arg_scripted, sizeof(arg_scripted)) == 0) {
				printf("doing scripted movement\n");
				scripted_movement = true;
				num_args++;
			}
		}

		if(num_args < argc-1) {
			printf("invalid argument\n");
			return EXIT_FAILURE;
		}
		if(!one_render) printf("continuous render\n");
		if(!scripted_movement) printf("key movement\n");
	}


	// init ncurses
	//std::unique_ptr<curse, curse::deleter> curse_ptr = curse::make_unique();
	std::shared_ptr<curse> curse_ptr;
	if(!scripted_movement) {// init curses
		curse_ptr = curse::make_shared();
		/* timeout sets the delay for getch()
		*  -1: block indefinitely
		*   0: don't block, give err instead
		*  >0: block for N ms, then give err
		*/
		timeout(-1);
		// makes input return immediately on every char, rather than buffering a line
		cbreak();
		// don't print characters
		noecho();
		// don't show the cursor
		curs_set(false);
		keypad(curse_ptr->window, true);
	}


	pthread_t mouse_id;
	pthread_t keys_id;
	{// start mouse and keyboard threads
		pthread_attr_t thread_attibutes;
		pthread_attr_init(&thread_attibutes);
		std::unique_ptr<pthread_attr_t, void(*)(void*)>
			attr(&thread_attibutes, (void(*)(void*))pthread_attr_destroy);

		pthread_attr_setdetachstate(attr.get(), PTHREAD_CREATE_JOINABLE);

		if(!scripted_movement) {
			std::string mouse_device = get_mouse_name((char*)"/proc/bus/input/devices");
			pthread_create(&mouse_id, attr.get(), mouse, &mouse_device);
			pthread_create(&keys_id, attr.get(), keys, &curse_ptr);
			curses_enabled = true;
		}
	}

	assert(!pthread_cond_wait(&main_start, &main_start_mutex));


#if 0// ncurses mouse test
	{
		std::unique_ptr<curse, curse::deleter> curse_ptr = curse::make_unique();
		noecho();
		curs_set(false);
		keypad(curse_ptr->window, true);

		mvprintw(1, 0, "Hello");
		mvprintw(2, 3, "World!");
		refresh();

		mmask_t my_settings = ALL_MOUSE_EVENTS;
		mousemask(my_settings, nullptr);

		std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
		std::chrono::duration<double> time_delta;
		do {
			time_delta = std::chrono::steady_clock::now() - start_time;
			mvprintw(0, 0, "time delta: %.0f", time_delta);

			MEVENT event;
			int ch = getch();
			if(ch == KEY_MOUSE || ch == KEY_MOVE) {
				if(getmouse(&event) == OK) {
					mvprintw(3, 0, "x: %d%10d", event.x, event.y);
					mvprintw(event.y, event.x, "o");
				} else {
					mvprintw(3, 0, "no event :(");
				}
			} else if(ch == ERR) {
				mvprintw(3, 0, "err");
			} else {
				mvprintw(3, 0, "not even a ch");
			}
			refresh();

			sleep(1);
		} while(time_delta.count() < 10);
	}
#endif

	/*
	 * setup framebuffer
	 * use a unique_ptr to delete it for us
	 */
	std::unique_ptr<framebuf, framebuf::deleter> fb = framebuf::make_unique();
	RGBT temp_rgbt = {255, 255, 255, 0};
	pixel_ pix(0, 0, temp_rgbt);


	// set up array of different directions for each pixel
#if USE_ARR_2D
	array_2d<v3d>
		pixel_dirs = make_pixel_arr(
				fb->vinfo.xres, fb->vinfo.yres, 90);

	{
		std::ofstream pixel_array_file("pixel_array.csv", std::ios::out);
		pixel_array_file << "x, y, radians" << std::endl;// title
		for(unsigned int x = 0; x < fb->vinfo.xres; x++)
			for(unsigned int y = 0; y < fb->vinfo.yres; y++) {
				pixel_array_file << x << ", " << y << ", " << pixel_dirs[x][y] << std::endl;
			}
	}
#else
	v3d *pixel_dirs = make_pixel_arr(fb->vinfo.xres, fb->vinfo.yres, 90);

	{
		std::ofstream pixel_array_file("pixel_array.csv", std::ios::out);
		pixel_array_file << "x, y, radians" << std::endl;// title
		for(unsigned int x = 0; x < fb->vinfo.xres; x++)
			for(unsigned int y = 0; y < fb->vinfo.yres; y++) {
				pixel_array_file << x << ", " << y << ", " << pixel_dirs[x + y*fb->vinfo.xres] << std::endl;
			}
	}
#endif


	// log file for the frame times
	std::shared_ptr<logger> frame_times_logger = nullptr;
	{
		std::ofstream frame_times_file("frame_times.csv", std::ios::out);
		if(frame_times_file) {
			frame_times_file << "micro seconds" << std::endl;// title
			frame_times_logger = std::make_shared<logger>(std::move(frame_times_file));
		}
	}


	// start the loop
	int frame_num = 0;
	size_t key = 0;
	script_keyframe before, after;
	quit_mutex.lock("main enter loop");
	while(!quit) {
		quit_mutex.unlock();

		// time this function please
		timer frame_timer = (frame_times_logger ? timer(frame_times_logger) : NULL);

		v3d dir, start;
		double hfov=0;
		if(scripted_movement) {
			/* linear interp the keyframes so we get the info for this frame
			 * set dir, start, hfov from scripted movement
			 */


			// figure out which keyframes we are in
			if(key+2 == script_len && script[key+1].frame_num == frame_num+1)
				break;

			assert(script[key].frame_num < script[key+1].frame_num);
			if(script[key+1].frame_num == frame_num) {
				key++;
				before = script[key];
				after = script[key+1];
			}

			// figure out the lerp
			double after_proportion =
				(double)(frame_num - script[key].frame_num) /
				(script[key+1].frame_num - script[key].frame_num);
			double before_proportion = 1 - after_proportion;

			if(!(before_proportion <= 1 && after_proportion <= 1)) {
				printf("\n===ERROR===\n");
				printf("proportions b:%1.2f a:%1.2f\n",
						before_proportion, after_proportion);
				printf("key: %lu\n", key);
				printf("frame_num: %d\n", frame_num);
				printf("keyframe frame_num curr:%d next:%d\n",
						script[key].frame_num, script[key+1].frame_num);
				assert(false);
			}

			// actually set the vars
			dir = script[key].dir * before_proportion +
				script[key+1].dir * after_proportion;
			dir.normalise();
			start = script[key].trans * before_proportion +
				script[key+1].trans * after_proportion;
			hfov = script[key].fov * before_proportion +
				script[key+1].fov * after_proportion;

		} else {// use vars from input
			direction_mutex.lock("main copy dir");
			dir = direction;
			direction_mutex.unlock();
			translation_mutex.lock("main copy translation");
			start = translation;
			translation_mutex.unlock();
			fov_mutex.lock("main copy fov");
			hfov = fov;
			fov_mutex.unlock();
		}

		m44d camera_transform;
		{
			// set up camera transformation matrix
			direction_mutex.lock("main copy dir for tx");
			camera_transform = rotate_tx;
			direction_mutex.unlock();

			translation_mutex.lock("setup camera transform");
			camera_transform *= m44d::make_translation(translation);
			translation_mutex.unlock();
		}


		// setup the fov
		double vfov = (hfov * fb->vinfo.yres) / fb->vinfo.xres;


		// calculate up and right directions
		v3d right;
		v3d up = v3d::Y;
		{
			v3d forward = dir;
			forward.y = 0; forward.normalise();
			right = v3d::cross(forward, up);
		}


		if(curses_enabled) {
			mvprintw(0, 0, "pos(%2.1f, %2.1f, %2.1f)", start.x, start.y, start.z);
			mvprintw(1, 0, "dir(%2.1f, %2.1f, %2.1f)", dir.x, dir.y, dir.z);
			refresh();
		}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
		// x gets converted to unsigned int, but all 
		for(int x = 0; x < fb->vinfo.xres && x < INT_MAX; x++) {
#pragma GCC diagnostic pop
			static_cast<PIXEL*>(&pix)->x = &x;

#if 0
			// account for difference in angle of different pixels
			double xr_on_2 = fb->vinfo.xres/2.0;
			double angle = hfov * (x - xr_on_2) / (double)xr_on_2;
			v3d pix_dir_x = v3d::rotate(dir, angle, up);
			v3d new_right = v3d::rotate(right, angle, up);
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			for(int y = 0; y < fb->vinfo.yres && y < INT_MAX; y++) {
#pragma GCC diagnostic pop
				static_cast<PIXEL*>(&pix)->y = &y;

				if(curses_enabled && x <= 200 && y <= 32)
					continue;
#if 0
				// angle of pixels, but for Y axis
				double yr_on_2 = fb->vinfo.yres/2.0;
				double angle = vfov * (y - yr_on_2) / (double)yr_on_2;
				v3d pix_dir_xy = v3d::rotate(pix_dir_x, angle, new_right);
				// TODO: use pythagoras to do both rotations in 1
#endif

#if USE_ARR_2D
				v3d dir2 = pixel_dirs[x][y];
#else
				v3d dir2 = pixel_dirs[x+fb->vinfo.xres*y];
#endif
				m41d dm(dir2, 0);
				v3d dir3 = camera_transform * dm;

				/*
				std::cout << std::endl << "dir: " << dir3 << std::endl;
				std::cout << std::endl << "dir: " << camera_transform << std::endl;
				direction_mutex.lock("read rotate tx");
				std::cout << std::endl << "dir: " << rotate_tx << std::endl;
				direction_mutex.unlock();
				*/

				unsigned int idx = 0;
				//HIT::type hit = ray_cast(start, pix_dir_xy, nullptr, &idx);
				HIT::type hit = ray_cast(start, dir3, nullptr, &idx);
				if(hit) {
					if(idx == 0) {
						pix.colour.r = 0;
						pix.colour.g = 255;
						pix.colour.b = 255;
						pix.colour.t = 255;
					} else {
						pix.colour.r = 0;
						pix.colour.g = 255;
						pix.colour.b = 0;
						pix.colour.t = 0;
					}
					draw(fb.get(), &pix);
				} else {
					pix.colour.r = (int)(255 * x) / fb->vinfo.xres;
					pix.colour.g = (int)(255 * y) / fb->vinfo.yres;
					pix.colour.b = 0;
					pix.colour.t = 0;
					draw(fb.get(), &pix);
				}
			}
		}

		if(one_render) break;

		frame_num++;
		quit_mutex.lock("main re-loop");
	}
	//quit_mutex.unlock();

	quit = true;
	quit_mutex.unlock();

	if(!scripted_movement) {
		pthread_join(mouse_id, nullptr);
		pthread_join(keys_id, nullptr);
	}

	return EXIT_SUCCESS;
}
