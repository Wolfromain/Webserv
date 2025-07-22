#!/usr/bin/env python3
import sys
import os

def main():
	print("Content-Type: text/html")
	print()
		
	print("<html><body>")
	print("<h1>Hello from CGI Python script!</h1>")
		
	query = os.environ.get('QUERY_STRING', '')
	print(f"<p>Query string: {query}</p>")
		
	print("</body></html>")

if __name__ == "__main__":
	main()
