#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <wsclient/wsclient.h>

typedef struct __wikimyei{
	char symbol[64];
	double price;
}__wikimyei_t;

#define TO_LOWER(in_upper) for(char *p=lower_str;*p;++p){*p=*p>='A'&&*p<='Z'?*p|0x60:*p;}

void suscriber(__wsclient_t *c) {
	char lower_str[64];
	strcpy(lower_str,((__wikimyei_t*)c->ref_struct)->symbol);
	TO_LOWER(lower_str);
	// for(char *p=lower_str;*p;++p){
	// 	*p=*p>='A'&&*p<='Z'?*p|0x60:*p;
	// }
	char *subscribe_str = malloc(1024*sizeof(char));
	// sprintf(subscribe_str,"{\"method\": \"SUBSCRIBE\",\"params\":[\"%s@miniTicker\",\"ethusdt@miniTicker\"],\"id\": 1}",lower_str);
	// sprintf(subscribe_str,"{\"method\": \"SUBSCRIBE\",\"params\":[\"%s@miniTicker\"],\"id\": 1}",((__wikimyei_t *)c->ref_struct)->symbol);
	// sprintf(subscribe_str,"{\"method\": \"SUBSCRIBE\",\"params\":[\"btcusdt@miniTicker\",\"btcupusdt@miniTicker\",\"btcdownusdt@miniTicker\"],\"id\": 72}",((__wikimyei_t *)c->ref_struct)->symbol);
	sprintf(subscribe_str,"{\"method\": \"SUBSCRIBE\",\"params\":[\"btcdownusdt@miniTicker\"],\"id\": 72}",((__wikimyei_t *)c->ref_struct)->symbol);
	fprintf(stdout,"subscribing : %s : %s \n",((__wikimyei_t*)c->ref_struct)->symbol,subscribe_str);
	libwsclient_send(c, subscribe_str, -1);
	free(subscribe_str);
}

int onclose(__wsclient_t *c) { // #FIXME attempt to reconect
	fprintf(stderr, "on_close called: %d\n", c->sockfd);
	// libwsclient_finish(client);
	// int libwsclient_open_connection(const char *host, const char *port) {
	// suscriber(c); // does not work, maybe add delay
	return 0;
}

int onerror(__wsclient_t *c, __wsclient_error_t *err) {
	fprintf(stderr, "on_error: (%d): %s\n", err->code, err->str);
	if(err->extra_code) {
		errno = err->extra_code;
		perror("recv");
	}
	return 0;
}

int onmessage(__wsclient_t *c, __wsclient_message_t *msg) {
	char *ch;
	char *sh;
	char *saveptr2;
	char filter_str[256];
	char *saveptr1;
	char *partial_str=malloc(strlen(msg->payload)+1);
	strcpy(partial_str,msg->payload);
	ch=strtok_r(partial_str, "{},", &saveptr1);
	if((sh = strstr(ch,"\"result\":null"))!=NULL){
		fprintf(stdout,"[cuwacunu:websocket] conextion stablished : %s\n",msg->payload);
		free(partial_str);
		return 0;
	}
    fprintf(stdout,"[cuwacunu:websocket] arriving message : %s\n",msg->payload);
	while(ch!=NULL){
		if((sh = strstr(ch,"\"e\":"))!=NULL){
			sh=strtok_r(sh, "\":", &saveptr2);
			sh=strtok_r(saveptr2, "\":", &saveptr2);
			// fprintf(stdout,"waka : [event type] : %s \n",sh);
		} else if ((sh = strstr(ch,"\"E\":"))!=NULL) {
			sh=strtok_r(sh, "\":", &saveptr2);
			sh=strtok_r(saveptr2, "\":", &saveptr2);
			// fprintf(stdout,"waka : [event time] : %s \n",sh);
		} else if ((sh = strstr(ch,"\"s\":"))!=NULL) {
			sh=strtok_r(sh, "\":", &saveptr2);
			sh=strtok_r(saveptr2, "\":", &saveptr2);
			// fprintf(stdout,"waka : [simbol] : %s \n",sh);
		} else if ((sh = strstr(ch,"\"c\":"))!=NULL) {
			sh=strtok_r(sh, "\":", &saveptr2);
			sh=strtok_r(saveptr2, "\":", &saveptr2);
			// fprintf(stdout,"waka : [close price] : %s \n",sh);
			((__wikimyei_t*)c->ref_struct)->price=strtof(sh,NULL);
		} else if ((sh = strstr(ch,"\"o\":"))!=NULL) {
			sh=strtok_r(sh, "\":", &saveptr2);
			sh=strtok_r(saveptr2, "\":", &saveptr2);
			// fprintf(stdout,"waka : [open price] : %s \n",sh);
		} else if ((sh = strstr(ch,"\"h\":"))!=NULL) {
			sh=strtok_r(sh, "\":", &saveptr2);
			sh=strtok_r(saveptr2, "\":", &saveptr2);
			// fprintf(stdout,"waka : [high price] : %s \n",sh);
		} else if ((sh = strstr(ch,"\"l\":"))!=NULL) {
			sh=strtok_r(sh, "\":", &saveptr2);
			sh=strtok_r(saveptr2, "\":", &saveptr2);
			// fprintf(stdout,"waka : [low price] : %s \n",sh);
		} else if ((sh = strstr(ch,"\"v\":"))!=NULL) {
			sh=strtok_r(sh, "\":", &saveptr2);
			sh=strtok_r(saveptr2, "\":", &saveptr2);
			// fprintf(stdout,"waka : [total traded base] : %s \n",sh);
		} else if ((sh = strstr(ch,"\"q\":"))!=NULL) {
			sh=strtok_r(sh, "\":", &saveptr2);
			sh=strtok_r(saveptr2, "\":", &saveptr2);
			// fprintf(stdout,"waka : [total traded quote] : %s \n",sh);
		} else{
			break;
		}
		ch=strtok_r(saveptr1, "{},", &saveptr1);
	}
	free(partial_str);
	return 0;
}

int onopen(__wsclient_t *c) {
	fprintf(stderr, "on_open called: [%d]\n", c->sockfd);
	suscriber(c);
	return 0;
}

int main(int argc, char **argv) {
	//Initialize new __wsclient_t * using specified URI
	__wsclient_t *client = libwsclient_new("wss://stream.binance.com:9443/ws");
	if(!client) {
		fprintf(stderr, "Unable to initialize new WS client.\n");
		exit(1);
	}
	//reference a general porpouse struct
	client->ref_struct = malloc(sizeof(__wikimyei_t));
	((__wikimyei_t*)client->ref_struct)->price=0x00;
	strcpy(((__wikimyei_t*)client->ref_struct)->symbol,"BTCUSDT");
	//set callback functions for this client
	libwsclient_onopen(client, &onopen);
	libwsclient_onmessage(client, &onmessage);
	libwsclient_onerror(client, &onerror);
	libwsclient_onclose(client, &onclose);
	//starts run thread.
	libwsclient_run(client);
	//blocks until run thread for client is done.
	pthread_join(client->run_thread, NULL);
	//finish
	free(client->ref_struct);
	libwsclient_finish(client);
	return 0;
}

