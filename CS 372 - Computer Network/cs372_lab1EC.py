import requests
result = requests.get("http://gaia.cs.umass.edu/wireshark-labs/INTRO-wireshark-file1.html")

headers = result.raw._original_response.msg._headers

# this prints the status code
print(result.status_code, " ", result.reason) 
for i in headers:
    print(i[0], ": ", i[1])

# I got helped from Stack Overflow and the class slide
print("\n", result.text)
