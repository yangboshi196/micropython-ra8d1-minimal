import sys
import os

def process_text(text):
    text = text.lower()
    punctuation = ",!?."
    for char in punctuation:
        text = text.replace(char,"")

    words = text.split()
    word_count = {}
    for word in words:
        word_count[word] = word_count.get(word,0) + 1

    return word_count


if __name__=="__main__":
  text = "AI is great. AI is the future!"
  result = process_text(text)
  print(result)


pass
