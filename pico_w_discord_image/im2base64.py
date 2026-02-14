import base64
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument('img')

args = parser.parse_args()

with open(args.img, "rb") as image_file:
    encoded_string = base64.b64encode(image_file.read()).decode("utf-8")

print(encoded_string)
