from PIL import Image, ImageDraw
from mandelbrot import mandelbrot, MAX_ITER

MAX_ITER = 80

def mandelbrot(c):
    z = 0
    n = 0
    while abs(z) <= 2 and n < MAX_ITER:
        z = z*z + c
        n += 1
    return n

# Image size (pixels)
WIDTH = 600
HEIGHT = 400

# Plot window
RE_START = -2
RE_END = 1
IM_START = -1
IM_END = 1

palette = []

im = Image.new('RGB', (WIDTH, HEIGHT), (0, 0, 0))
draw = ImageDraw.Draw(im)

for x in range(0, WIDTH):
    for y in range(0, HEIGHT):
        # Convert pixel coordinate to complex number
        c = complex(RE_START + (x / WIDTH) * (RE_END - RE_START),
                    IM_START + (y / HEIGHT) * (IM_END - IM_START))
        # Compute the number of iterations
        m = mandelbrot(c)
        # The color depends on the number of iterations
        color = 255 - int(m * 255 / MAX_ITER)
        # Plot the point
        draw.point([x, y], (color, color, color))

im.save('output.png', 'PNG')






'''
x = -2
y = -1

z0 = complex(x, y)
print("Start", z0)

#print("pow", pow(z0, 2))

#print("add", complex(3,4) + complex(-2, -1))

for index in range(10):
    zn = pow(z0, 2) + complex(x,y)
    #print(index, zn)


print("Ende", zn)
'''