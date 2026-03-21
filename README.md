# 🚗 Vehicle OTA Platform (Embedded Linux + C)

## 🔥 Overview
This project implements a **production-grade Over-The-Air (OTA) firmware update system** inspired by modern automotive platforms.

It simulates how ECUs in vehicles securely receive, validate, and install firmware updates from the cloud.

---

## 🏗️ Architecture
+------------------+ +----------------------+
| OTA Server | ----> | OTA Daemon (C) |
| (Flask + JSON) | | Embedded Linux |
+------------------+ +----------------------+
| |
| v
| Firmware Download
| |
| SHA256 Verification
| |
| A/B Slot Installation
| |
| Rollback on Failure


---

## ⚙️ Features

- ✅ OTA client daemon in **C (Embedded Linux)**
- ✅ HTTP communication using **libcurl**
- ✅ JSON-based OTA server using **Flask**
- ✅ Firmware download & storage
- ✅ **SHA256 validation using OpenSSL EVP API**
- ✅ **A/B partitioning system**
- ✅ Fail-safe **rollback mechanism**
- ✅ Modular architecture (production-style)

---

## 🔄 OTA Flow

1. Device queries OTA server
2. Server responds with firmware URL + hash
3. Firmware is downloaded via HTTP
4. SHA256 hash is computed
5. Hash is compared with server value
6. Firmware installed in inactive slot
7. Slot switch performed
8. Rollback if failure detected

---

## 🛠️ Tech Stack

- **C (Embedded Linux)**
- **Python (Flask)**
- **libcurl**
- **OpenSSL (EVP SHA256)**
- **WSL / Linux Environment**

---

## 🚀 How to Run

### Start Server
```bash
python3 ota_server.py


### Run OTA DAEMON
cd ota_daemon
make
./ota-daemon

🔐 Security Highlights
Firmware integrity ensured using SHA256
Prevents tampered/corrupted updates
Fail-safe A/B update mechanism

📈 Future Improvements
TLS certificate validation
Secure boot integration
Delta OTA updates
systemd service integration
ECU-level flashing simulation
👨‍💻 Author

Harikrishna Singh Bhadauria
Software Engineer 








