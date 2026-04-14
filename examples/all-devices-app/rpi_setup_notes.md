# Raspberry Pi Setup Notes for all-devices-app

## 1. Systemd Service Setup
To run the `all-devices-app` automatically on boot, create a systemd service file.

**File path:** `/etc/systemd/system/all-devices-app.service`
**Creation command:** `sudo nano /etc/systemd/system/all-devices-app.service`

**File Content:**
```ini
[Unit]
Description=Matter All Devices App (Chime)
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
User=ubuntu
Group=ubuntu

WorkingDirectory=/home/ubuntu
ExecStart=/home/ubuntu/all-devices-app --device chime --KVS /home/ubuntu/chip_kvs

Restart=always
RestartSec=5

SyslogIdentifier=matter-chime

[Install]
WantedBy=multi-user.target
```

**Commands to enable and start the service:**
```bash
sudo systemctl daemon-reload
sudo systemctl enable all-devices-app.service
sudo systemctl start all-devices-app.service
```

**Commands to check status and logs:**
```bash
sudo systemctl status all-devices-app.service
sudo journalctl -u all-devices-app.service -f
```

---

## 2. Audio Configuration (ALSA)

### Default Audio Device
To set the default audio device to a USB sound card (card 1) for the `ubuntu` user, the `~/.asoundrc` file is used.

**File path:** `~/.asoundrc`
**Content:**
```
pcm.!default {
    type hw
    card 1
}

ctl.!default {
    type hw
    card 1
}
```
*Note: Because the systemd service runs as `User=ubuntu`, it will read this user-specific configuration.*

### Adjusting Volume
Because the USB sound card is card 1, the default ALSA commands might adjust the wrong card. Use the `-c 1` flag.

**Interactive Mixer:**
```bash
alsamixer -c 1
```
*(Use Up/Down arrows to adjust, `m` to unmute if `[MM]` is shown, `Esc` to exit).*

**Command Line (Example):**
```bash
amixer -c 1 sset 'Speaker' 100%
```
*(You may need to replace 'Speaker' with 'Master', 'PCM', or 'Headphone' depending on your USB adapter).*

### Saving Volume Settings
To ensure the volume settings persist across reboots, save the ALSA state:
```bash
sudo alsactl store
```
