#ifndef UPTIME_H
#define UPTIME_H

class uptime
{
public:
  int m_mqttTopic;
  int m_uptime;
  
	uptime(int topic);
	void getUptime(void);
};

#endif

