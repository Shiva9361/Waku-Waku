FROM python:3.10

WORKDIR /app

COPY ./src/gui/backend ./src
COPY ./codes ./codes

ADD requirements.txt .

RUN pip3 install -r requirements.txt

CMD ["python3","./src/main.py"]