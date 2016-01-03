#ifndef UPTIME_H
#define UPTIME_H

class uptime
{
private:
	int m_mqttTopic;
	int m_uptime;
	bool m_sendFlag;

public:
	uptime(int topic);
	void getUptime(void);
  void sendIfChanged();
};

#endif

