FROM python:3.10

WORKDIR /app

COPY ./src/gui/backend ./src

ADD requirements.txt .

RUN pip3 install -r requirements.txt

EXPOSE 8000

CMD ["gunicorn","--bind", "0.0.0.0:5000","-w","4","--chdir" ,"src/", "main:app"]