/*************************************************************************
    > File Name: connection.h
    > Author: Jiange
    > Mail: jiangezh@qq.com 
    > Created Time: 2016年01月27日 星期三 20时10分35秒
 ************************************************************************/

#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <string>
#include <queue>

#include "event2/event.h"
#include "event2/util.h"

#include "util.h"
#include "http.h"
#include "plugin.h"

typedef enum
{
	CON_STATE_CONNECT,
	CON_STATE_REQUEST_START,
	CON_STATE_READ,
	CON_STATE_REQUEST_END,
	CON_STATE_HANDLE_REQUEST,
	CON_STATE_RESPONSE_START,
	CON_STATE_WRITE,
	CON_STATE_RESPONSE_END,
	CON_STATE_ERROR
} connection_state_t;

typedef enum
{
    REQ_ERROR,
    REQ_IS_COMPLETE,
    REQ_NOT_COMPLETE
} request_state_t;

class Worker;

class Connection
{
	public:
	    typedef std::queue<HttpRequest*> req_queue_t;

		Connection();
		~Connection();

		bool InitConnection(Worker *worker);
		static void ConEventCallback(evutil_socket_t fd, short event, void *arg);
		static void FreeConnection(Connection *con);

		Worker			   *con_worker;

		evutil_socket_t		con_sockfd;
		struct sockaddr_in	con_addr;
		struct event	   *con_event;				//这里用两个event，一个注册读，一个注册写会效率高点
		//struct event	   *write_event;
		//struct event	   *read_event;

		std::string			con_inbuf;
		std::string			con_intmp;
		std::string			con_outbuf;

		req_queue_t			req_queue;

	    bool				con_want_write;
	    bool				con_want_read;
		bool				con_keep_alive;
		int 				con_req_cnt;

		void*			   *plugin_data_slots;
		int					plugin_cnt;
		int                	plugin_next;

	    HttpRequest		   *http_req_parser;  	//解析时用
	    HttpRequest		   *http_req_parsed;   //处理请求时用
	    HttpResponse		http_response;
	    HttpParser			http_parser;

	private:
    	void WantRead();
    	void NotWantRead();
    	void WantWrite();
    	void NotWantWrite();
    	void SetWriteEvent();
    	void UnsetWriteEvent();
		void ResetConnection();
		void PrepareResponse();
		void SetErrorResponse();
		bool StateMachine();
    	void SetState(connection_state_t state);
    	request_state_t GetParsedRequest();

		bool InitPluginDataSlots();
    	void FreePluginDataSlots();

    	bool PluginRequestStart();
    	bool PluginRead();
    	bool PluginRequestEnd();
    	bool PluginResponseStart();
    	plugin_state_t PluginWrite();
    	bool PluginResponseEnd();

		connection_state_t	con_state;
		request_state_t		req_state;

};

#endif
