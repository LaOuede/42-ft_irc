import socket
import time

def test_firewall(host, port, iterations, delay):
	for i in range(iterations):
		try:
			print(f"Connecting attempt {i+1} to {host}:{port}")
			with socket.create_connection((host, port), timeout=delay) as s:
				print("Connected!")
				time.sleep(3) # sleep avant d'envoyer un message après s'être connecté
				s.sendall(b'PASS LIBERTE EGALITE FRATERNITE\r\n')
				print("PASS command sent.")

		except socket.timeout:
			print("Connection attempt timed out.")
		except Exception as e:
			print(f"An error occurred: {e}")
		finally:
			time.sleep(delay) # Sleep avant de deco
			print("Disconnecting")

if __name__ == "__main__":
	HOST = 'c2r5p3.42quebec.com'  # Replace with your host
	PORT = 6667                   # Replace with your port
	ITERATIONS = 5
	DELAY = 2                     # Delay in seconds

	test_firewall(HOST, PORT, ITERATIONS, DELAY)