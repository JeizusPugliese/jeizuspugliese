from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
import time

# Configuración del navegador
driver = webdriver.Chrome(executable_path='path_to_chromedriver')  # Asegúrate de tener el chromedriver adecuado
driver.get('https://olymptrade.com')

# Iniciar sesión
login_button = driver.find_element(By.XPATH, 'path_to_login_button')  # Encuentra el botón de inicio de sesión
login_button.click()

username_input = driver.find_element(By.XPATH, 'path_to_username_input')
password_input = driver.find_element(By.XPATH, 'path_to_password_input')

username_input.send_keys('jesuspugliese2@gmail.com')
password_input.send_keys('Pugliee1302')
password_input.send_keys(Keys.RETURN)

# Espera para la autenticación
time.sleep(10)

# Navegar y realizar acciones
# Encuentra elementos y realiza operaciones según tu estrategia

# Cerrar el navegador
driver.quit()
