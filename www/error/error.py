import sys

def str_cat(x):
	result = "concatinated string: " + x


if __name__ == "__main__":
    x = 0
    try:
        str_cat(x)
    except:
        sys.exit(1)
