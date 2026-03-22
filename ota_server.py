import hashlib
from flask import Flask, jsonify, send_file

app = Flask(__name__)

FIRMWARE_PATH = "ota_daemon/firmware.bin"

def calculate_hash(file_path):
    sha256 = hashlib.sha256()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            sha256.update(chunk)
    return sha256.hexdigest()

@app.route("/update")
def update():
    with open("firmware.sig", "rb") as f:
        signature = f.read().hex()

    return jsonify({
        "version": "1.0.1",
        "url": "https://127.0.0.1:5000/firmware",
        "hash": calculate_hash(FIRMWARE_PATH),
        "signature": signature
    })
@app.route("/firmware")
def firmware():
    return send_file(FIRMWARE_PATH, as_attachment=True)

if __name__ == "__main__":
    app.run(
    host="127.0.0.1",
    port=5000,
    ssl_context=(
        "/mnt/d/Embedded_Linux/Project/vehicle-ota-platform-qemu/certs/cert.pem",
        "/mnt/d/Embedded_Linux/Project/vehicle-ota-platform-qemu/certs/key.pem"
    )
)
