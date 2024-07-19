import sys

def increment(x):
    while (True):
        x += 1

if __name__ == "__main__":
    x = 0
    try:
        increment(0)
    except:
        sys.exit(1)
