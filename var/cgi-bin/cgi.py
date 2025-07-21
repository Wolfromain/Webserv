#!/usr/bin/env python
import os
import sys

# Lire les données envoyées par POST
if os.environ.get('REQUEST_METHOD', '') == 'POST':
	content_length = int(os.environ.get('CONTENT_LENGTH', 0))
	post_data = sys.stdin.read(content_length)
else:
	post_data = ''

# Lire la query string (pour GET)
query_string = os.environ.get('QUERY_STRING', '')

# Afficher les en-têtes CGI
print("Content-Type: text/html")
print("Status: 200 OK")
print()  # Fin des headers

# Corps HTML (body)
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>Hello from CGI!</h1>")
print("<p>Query String: {}</p>".format(query_string))
print("<p>POST Data: {}</p>".format(post_data))
print("</body>")
print("</html>")