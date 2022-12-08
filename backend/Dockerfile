FROM python:3.9.16-slim-bullseye

EXPOSE 5000

COPY . /app
WORKDIR /app

RUN pip install -r requirements.txt
CMD ["python", "app.py"]