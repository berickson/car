import pandas as pd
from flask import Flask

app = Flask(__name__)

@app.route("/")
def index():
    return "index"

def not_called():
  df = pd.read_csv('abc')
  return df.to_json()

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)
