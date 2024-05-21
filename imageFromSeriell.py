import serial
from PIL import Image
import numpy as np
import serial
from PIL import Image, ImageEnhance
import io
import matplotlib.pyplot as plt

# Chose Usb Port
serialInst = serial.Serial()
serialInst.baudrate = 115200
serialInst.port = 'COM3'
# ESP anschliessen
serialInst.open()
serialInst.write(b'OK')
# Bildgroesse lesen
image_size = int.from_bytes(serialInst.read(4), byteorder='little')
# Paritaetsbit empfangen
received_parity_bit = serialInst.read(1)
# Bilddaten lesen
image_data = serialInst.read(image_size)
print(type(image_data))
# Paritaetsbit berechnen
calculated_parity_bit = 0
for i in range(32):
    calculated_parity_bit ^= (image_size >> i) & 1
# ueberpruefen Paritaetsbit
if received_parity_bit[0] == calculated_parity_bit:
    try:
        # Bild aus den Bilddaten laden
        img = Image.open(io.BytesIO(image_data))
        output_filename = 'image.jpg'

        brightness = ImageEnhance.Brightness(img)
        contrast = ImageEnhance.Contrast(img)
        # img = contrast.enhance(2.0)# Kontrast erhoehen
        img = brightness.enhance(0.8)# helligkeit reduzieren

        # Bild speichern
        img.save(output_filename)
        print(f"Bild wurde als {output_filename} gespeichert und erfolgreich geöffnet.")
    except Exception as e:
        print(f"Fehler beim Öffnen oder Speichern des Bildes: {e}")
else:
 print("Fehler: Paritätsbit stimmt nicht überein!")

serialInst.close()