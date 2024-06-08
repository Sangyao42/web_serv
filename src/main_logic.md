```
if (!clt->continue_reading):
	parse request
void preprocess_headers(clt){
	//update status_code
	//update clt->consume_body = false when error occurs
	//check if chunked, update clt->is_chunked = true
}

if (is_chunk):
	clt->continue_reading = false;
	do {
		//update clt->continue_reading
	} while(chunk_size > 0);
	IgnoreEntityHeaders(clt->client_socket->req_buf)
	if (!CheckCrlf)
		clt->status_code = k400
		clt->keelAlive = false
		ProcessRequest(clt)
		pfds[i].events = POLLOUT
		continue
	if (clt->continue_reading):
		continue
	if (clt->exceed_max_body_size):
		clt->status_code = k413
		clt->keepAlive = false
	ProcessRequest(clt)
	pfds[i].events = POLLOUT

else: // no chunk, use Content-Length.
	if (!clt->contineu_reading):
		clt->content_length = A_NUM
		if (A_NUM exceed_max_body_size):
			clt->status_code = k413
			clt->keepAlive = false
			ProcessRequest(clt)
			pfds[i].events = POLLOUT
			//continue
	clt->continue_reading = false
	if (A_NUM > len(clt->client_socket->req_buf)):
		clt->continue_reading = true
		continue
	if (cosume_body):
		std::string content_body = clt->client_socket->req_buf.substr(0, A_NUM);
		clt.req.body.append(content_body)
	clt->client_socket->req_buf.erase(0, A_NUM)
	ProcessRequest(clt)
	pfds[i].events = POLLOUT
	//continue




```
