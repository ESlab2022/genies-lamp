FROM python:3.9.16-slim-bullseye

EXPOSE 5000

COPY . /app
WORKDIR /app

RUN apt-get update && apt-get -y install libpq-dev gcc && pip install psycopg2
RUN pip install -r requirements-prod.txt

CMD ["python", "app.py"]