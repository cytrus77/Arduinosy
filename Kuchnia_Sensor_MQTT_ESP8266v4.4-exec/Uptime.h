#ifndef UPTIME_H
#define UPTIME_H

class uptime
{
private:
	int m_mqttTopic;
	int m_uptime;
	int m_sendFlag;

public:
	uptime(int topic);
	void getUptime(void);
};

#endif

