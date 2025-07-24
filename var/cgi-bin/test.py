#!/usr/bin.python3
import cgi
import os
import html
import shutil
import sys

print("Content-Type: text/html\n")

form = cgi.FieldStorage()

name = form.getvalue("name", "").strip()

if "image" in form:
    fileitem = form["image"]
else:
    fileitem = None

print("<html><head><title>Résultat</title></head><body>")
print("<h1>Résultat de l'envoi</h1>")

if not name:
    print("<p style='color: red;'>Erreur : Pseudo manquant.</p>")

elif fileitem is None or not fileitem.filename:
    # fileitem est None OU pas de fichier sélectionné
    print("<p style='color: red;'>Erreur : Aucune image sélectionnée.</p>")

else:
    safe_name = html.escape(name)
    filename = os.path.basename(fileitem.filename)
    filepath = f"/tmp/{filename}"

    with open(filepath, 'wb') as f:
        shutil.copyfileobj(fileitem.file, f)

    print(f"<p>Pseudo Minecraft : <strong>{safe_name}</strong></p>")
    print(f"<p>Image reçue : <strong>{filename}</strong></p>")
    print(f"<p>Fichier enregistré dans : <code>{filepath}</code></p>")
    print(f'<img src="file://{filepath}" alt="Image envoyée" width="200">')

print("</body></html>")
sys.stdout.flush()