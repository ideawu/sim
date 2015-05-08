#ifndef UTIL_APP_H
#define UTIL_APP_H

#include <string>

namespace sim{

class Config;

class Application{
public:
	Application(){};
	virtual ~Application(){};

	int main(int argc, char **argv);
	
	virtual void usage(int argc, char **argv);
	virtual void welcome();
	virtual int init();
	virtual void free();
	virtual void loop_once() = 0;

protected:
	struct AppArgs{
		bool is_daemon;
		std::string pidfile;
		std::string conf_file;
		std::string work_dir;
		std::string start_opt;

		AppArgs(){
			is_daemon = false;
			start_opt = "start";
		}
	};

	Config *conf;
	AppArgs app_args;
	
private:
	void parse_args(int argc, char **argv);
	void my_init();

	int read_pid();
	void write_pid();
	void check_pidfile();
	void remove_pidfile();
	void kill_process();
};


}; // namespace sim

#endif
