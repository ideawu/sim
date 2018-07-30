#ifndef SIM_LINK_H
#define SIM_LINK_H

class Link
{
public:
	Link();

	int fd() const;
	
	bool is_new() const;
	bool is_closed() const;
	bool is_working() const;
	
	friend class Transport;
	
protected:
	int _status;

private:
	virtual ~Link();

	int _fd;
	
	void accept();
	void close();
};

#endif
