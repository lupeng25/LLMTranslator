from PIL import Image, ImageDraw, ImageFont
import os

def create_icon():
    # Size settings
    size = (256, 256)
    
    # Create a new image with transparency
    img = Image.new('RGBA', size, (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw a rounded rectangle (Indigo background)
    # Since PIL doesn't have native rounded rectangle with anti-aliasing easily, 
    # we'll draw a rectangle and cut corners or just use a standard rectangle for simplicity
    # or draw a circle. Let's do a rounded square style.
    
    bg_color = (99, 102, 241) # Indigo #6366F1
    
    # Draw main background
    # padding
    p = 10
    draw.rounded_rectangle([p, p, size[0]-p, size[1]-p], radius=40, fill=bg_color)
    
    # Draw Text "文" (Translation symbol)
    # We need a font. We'll try to use a default font or load one.
    # Since we can't guarantee a font file, we'll draw simple shapes to represent "A" and "文"
    # Or just "LLM"
    
    # Let's draw "LLM" in white
    text_color = (255, 255, 255)
    
    # Draw simple lines for "L"
    # L1
    # Vertical
    w = 20 # stroke width
    
    # Draw "文" using lines/shapes to be font-independent
    # Top dot
    # Horizontal line
    # Curve strokes... too complex to draw manually without font.
    
    # Let's draw "Tr" (Translator)
    # Or simply "A" (AI)
    
    # Let's try to load a default font
    try:
        # Try arial or similar
        font = ImageFont.truetype("arial.ttf", 140)
        text = "文"
        # Center text
        # getbbox is cleaner
        bbox = draw.textbbox((0, 0), text, font=font)
        text_w = bbox[2] - bbox[0]
        text_h = bbox[3] - bbox[1]
        
        x = (size[0] - text_w) / 2
        y = (size[1] - text_h) / 2 - 20 # Move up slightly
        
        draw.text((x, y), text, font=font, fill=text_color)
        
    except Exception as e:
        print(f"Font load failed: {e}, drawing fallback shapes")
        # Fallback: Draw a white circle
        draw.ellipse([60, 60, 196, 196], fill=text_color)

    # Save as ICO
    # We need to save multiple sizes for a good ICO
    icon_sizes = [(256, 256), (128, 128), (64, 64), (48, 48), (32, 32), (16, 16)]
    img.save('app_icon.ico', format='ICO', sizes=icon_sizes)
    print("app_icon.ico created successfully")

if __name__ == "__main__":
    create_icon()
