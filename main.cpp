#include "main.h"

namespace HIT {
	enum type {
		undef = 0,
		sphere = 1,
	};
}

typedef struct {
	v3d c;//centre
	double r;//radius
} sphere;

sphere spheres[] = {
	{v3d(0,0,0), 5},
	{v3d(-4, 2.5, -3), 1},
};
unsigned int num_spheres = sizeof(spheres) / sizeof(spheres[0]);

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
				hit = HIT::sphere;

				double sqrt_discrim = sqrt(discriminant);
				d = fmin(-lc_to_o + sqrt_discrim, -lc_to_o - sqrt_discrim);

				if(d < min_dist) {
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

class curse {
	public:
		struct deleter {
			void operator()(curse* __ptr) const {
				endwin();
			};
			void end(void *__ptr) const {
				endwin();
			};
		};

		WINDOW *window;

		static std::unique_ptr<curse, curse::deleter> make_unique() {
			return std::unique_ptr<curse, deleter>(
					new curse(initscr()), deleter());
		};

		static std::shared_ptr<curse> make_shared() {
			return std::shared_ptr<curse>(new curse(initscr()), deleter());
		};

	protected:
		// don't want anything to call this
		curse(WINDOW *window) :window(window) {};
};

std::string get_mouse_name(char *devices_list) {

	bool found_event_name = false;
	std::string event_name;
	do {
		FILE *stream;
		//char devices_list[] = "/proc/bus/input/devices";

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
						break;
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

pthread_mutex_t direction_mutex = PTHREAD_MUTEX_INITIALIZER;
v3d direction(1,0,0);
pthread_mutex_t translation_mutex = PTHREAD_MUTEX_INITIALIZER;
v3d translation(-13, 2, -3);

pthread_mutex_t fov_mutex = PTHREAD_MUTEX_INITIALIZER;
double fov = 90;

pthread_mutex_t quit_mutex = PTHREAD_MUTEX_INITIALIZER;
bool quit = false;

const double units_per_sec = 2;


void* keys(void *curse_shared_ptr) {
	std::shared_ptr<curse> curses_ptr = *(std::shared_ptr<curse>*)(curse_shared_ptr);

	//time_point<steady_clock> start_time = steady_clock::now();
	//std::chrono::duration<double> time_delta;
	bool loop = true;

	time_point<steady_clock> current_time = steady_clock::now();
	do {
		//time_delta = steady_clock::now() - start_time;
		//mvprintw(0, 0, "time delta: %.0f", time_delta);


		time_point<steady_clock> last_time = current_time;
		current_time = steady_clock::now();


		int key = getch();

		// get the 3 unit vectors relative to direction
		assert(!pthread_mutex_lock(&direction_mutex));
		v3d forward = direction;
		assert(!pthread_mutex_unlock(&direction_mutex));
		// set forward vector to be not looking up or down
		forward.y = 0; forward.normalise();
		v3d up = v3d::Y;
		v3d right = v3d::cross(forward, up);

		duration<double> delta;
		switch(key) {
			case KEY_UP:
			case (int)'w':
				delta = current_time - last_time;
				assert(!pthread_mutex_lock(&translation_mutex));
				translation += forward * units_per_sec * delta.count();
				assert(!pthread_mutex_unlock(&translation_mutex));
				break;
			case KEY_DOWN:
			case (int)'s':
				delta = current_time - last_time;
				assert(!pthread_mutex_lock(&translation_mutex));
				translation -= forward * units_per_sec * delta.count();
				assert(!pthread_mutex_unlock(&translation_mutex));
				break;
			case KEY_RIGHT:
			case (int)'d':
				delta = current_time - last_time;
				assert(!pthread_mutex_lock(&translation_mutex));
				translation += right * units_per_sec * delta.count();
				assert(!pthread_mutex_unlock(&translation_mutex));
				break;
			case KEY_LEFT:
			case (int)'a':
				delta = current_time - last_time;
				assert(!pthread_mutex_lock(&translation_mutex));
				translation -= right * units_per_sec * delta.count();
				assert(!pthread_mutex_unlock(&translation_mutex));
				break;

			case (int)' ':
				delta = current_time - last_time;
				assert(!pthread_mutex_lock(&translation_mutex));
				translation += up * units_per_sec * delta.count();
				assert(!pthread_mutex_unlock(&translation_mutex));
				break;
			case (int)'c':
				delta = current_time - last_time;
				assert(!pthread_mutex_lock(&translation_mutex));
				translation -= up * units_per_sec * delta.count();
				assert(!pthread_mutex_unlock(&translation_mutex));
				break;

			case (int)'+':
			case (int)'='://no shift held
				assert(!pthread_mutex_lock(&fov_mutex));
				fov *= 1.5;
				assert(!pthread_mutex_unlock(&fov_mutex));
				break;
			case (int)'-':
			case (int)'_'://shift held
				assert(!pthread_mutex_lock(&fov_mutex));
				fov /= 1.5;
				assert(!pthread_mutex_unlock(&fov_mutex));
				break;



			case 'q':
			case KEY_EXIT:
				assert(!pthread_mutex_lock(&quit_mutex));
				quit = true;
				loop = false;
				assert(!pthread_mutex_unlock(&quit_mutex));
				break;
		}

	} while(loop);

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
		exit(EXIT_FAILURE);
	}

	double x_deg = 0;
	double y_deg = 0;

	while(read(fd, &ie, sizeof(struct input_event))) {
		//printf("time %ld.%06ld\ttype %d\tcode %d\tvalue %d\n",
		//    ie.time.tv_sec, ie.time.tv_usec, ie.type, ie.code, ie.value);

		assert(!pthread_mutex_lock(&quit_mutex));
		if(quit) {
			assert(!pthread_mutex_unlock(&quit_mutex));
			break;
		}
		assert(!pthread_mutex_unlock(&quit_mutex));

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
		assert(!pthread_mutex_lock(&direction_mutex));

		direction = 
			v3d::rotate(v3d::X, y_deg, -v3d::Z)
			.rotate(x_deg, -v3d::Y);

		assert(!pthread_mutex_unlock(&direction_mutex));
	}

	return NULL;
}


int main() {
	timer main_timer("main timer");

	// init ncurses
	//std::unique_ptr<curse, curse::deleter> curse_ptr = curse::make_unique();
	std::shared_ptr<curse> curse_ptr = curse::make_shared();
	noecho();
	curs_set(false);
	keypad(curse_ptr->window, true);

	std::string mouse_device = get_mouse_name((char*)"/proc/bus/input/devices");


	pthread_t mouse_id;
	pthread_create(&mouse_id, NULL, mouse, &mouse_device);
	pthread_t keys_id;
	pthread_create(&keys_id, NULL, keys, &curse_ptr);



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

		time_point<steady_clock> start_time = steady_clock::now();
		std::chrono::duration<double> time_delta;
		do {
			time_delta = steady_clock::now() - start_time;
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


	// start the loop
	while(1) {

		// check the controls set by other threads
		assert(!pthread_mutex_lock(&quit_mutex));
		if(quit) {
			assert(!pthread_mutex_unlock(&quit_mutex));
			break;
		}
		assert(!pthread_mutex_unlock(&quit_mutex));
		assert(!pthread_mutex_lock(&direction_mutex));
		v3d dir = direction;
		assert(!pthread_mutex_unlock(&direction_mutex));
		assert(!pthread_mutex_lock(&translation_mutex));
		v3d start = translation;
		assert(!pthread_mutex_unlock(&translation_mutex));
		assert(!pthread_mutex_lock(&fov_mutex));
		double hfov = fov;
		assert(!pthread_mutex_unlock(&fov_mutex));

		// setup the fov
		double vfov = (hfov * fb->vinfo.yres) / fb->vinfo.xres;


		// calculate up and right directions
		v3d right;
		v3d up = v3d::Y;
		{
			v3d forward = dir;;
			forward.y = 0; forward.normalise();
			right = v3d::cross(forward, up);
		}


		mvprintw(0, 0, "pos(%2.1f, %2.1f, %2.1f)", start.x, start.y, start.z);
		mvprintw(1, 0, "dir(%2.1f, %2.1f, %2.1f)", dir.x, dir.y, dir.z);
		refresh();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
		// x gets converted to unsigned int, but all 
		for(int x = 0; x < fb->vinfo.xres && x < INT_MAX; x++) {
#pragma GCC diagnostic pop
			static_cast<PIXEL*>(&pix)->x = &x;

			// account for difference in angle of different pixels
			double xr_on_2 = fb->vinfo.xres/2.0;
			double angle = hfov * (x - xr_on_2) / (double)xr_on_2;
			v3d pix_dir_x = v3d::rotate(dir, angle, up);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			for(int y = 0; y < fb->vinfo.yres && y < INT_MAX; y++) {
#pragma GCC diagnostic pop
				static_cast<PIXEL*>(&pix)->y = &y;

				if(x <= 200 && y <= 32)
					continue;

				// angle of pixels, but for Y axis
				double yr_on_2 = fb->vinfo.yres/2.0;
				double angle = vfov * (y - yr_on_2) / (double)yr_on_2;
				v3d pix_dir_xy = v3d::rotate(pix_dir_x, angle, right);
				// TODO: use pythagoras to do both rotations in 1

				unsigned int idx = 0;
				HIT::type hit = ray_cast(start, pix_dir_xy, nullptr, &idx);
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
	}

	return EXIT_SUCCESS;
}
