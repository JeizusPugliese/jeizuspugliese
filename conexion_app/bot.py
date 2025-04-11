import yfinance as yf
import numpy as np
import smtplib
from email.mime.text import MIMEText
import time

# Configuración de correo para enviar señales
def send_email(signal, recipient_email):
    msg = MIMEText(f'Nueva señal de trading: {signal}')
    msg['Subject'] = 'Señal de Trading'
    msg['From'] = 'jeyzus13@gmail.com'
    msg['To'] = recipient_email

    with smtplib.SMTP('smtp.gmail.com', 587) as server:
        server.starttls()
        server.login('jeyzus13@gmail.com', 'jesus1302')
        server.sendmail('jeyzus13@gmail.com', recipient_email, msg.as_string())

# Función principal del bot
def trading_bot():
    while True:
        # Descargar datos del Europe Composite Index (STOXX Europe 600)
        data = yf.download('USD', period='1d', interval='1m')  # Cambia '1m' a '5s' si es posible

        # Definir ventanas para medias móviles
        short_window = 5
        long_window = 10

        # Calcular medias móviles
        data['short_mavg'] = data['Close'].rolling(window=short_window, min_periods=1).mean()
        data['long_mavg'] = data['Close'].rolling(window=long_window, min_periods=1).mean()

        # Crear la columna 'signal' antes de asignarle valores
        data['signal'] = 0.0

        # Generar señales asegurando que las longitudes coincidan
        data['signal'] = np.where(data['short_mavg'] > data['long_mavg'], 1.0, 0.0)
        data['positions'] = data['signal'].diff()

        # Revisar las señales generadas
        latest_signal = data['positions'].iloc[-1]

        # Enviar señal por correo si hay cambio de posición
        if latest_signal == 1.0:
            send_email('Arriba', 'jeyzus13@gmail.com')
            print("Señal de 'Arriba' enviada")
        elif latest_signal == -1.0:
            send_email('Abajo', 'jeyzus13@gmail.com')
            print("Señal de 'Abajo' enviada")
        else:
            print("No hay señales nuevas")

        # Esperar 5 segundos antes de volver a consultar datos
        time.sleep(5)

if __name__ == "__main__":
    trading_bot()
