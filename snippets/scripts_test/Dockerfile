FROM nginx

RUN rm /etc/nginx/conf.d/default.conf
COPY www /usr/share/nginx/html
COPY ./conf/nginx.conf /etc/nginx/conf.d/default.conf

CMD ["nginx", "-g", "daemon off;"]
