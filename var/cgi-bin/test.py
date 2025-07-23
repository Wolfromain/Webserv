#!/usr/bin/env python3
import sys
import os
import cgi

print("Content-type: text/html\n")
print("<html><body>")
print("<h2>Environnement CGI:</h2>")
for k, v in os.environ.items():
    print(f"{k}: {v}<br>")
print(os.environ.get("QUERY_STRING", ""))
form = cgi.FieldStorage()
name = form.getvalue("name", "")
email = form.getvalue("email", "")
message = form.getvalue("message", "")

print(f"<h2>Nom: {name}</h2>")
print(f"<h2>Email: {email}</h2>")
print(f"<h2>Message: {message}</h2>")
print(f"</body></html>")