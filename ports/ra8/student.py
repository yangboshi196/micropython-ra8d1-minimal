import pandas as pd

students = [
{"name": "Alice", "math": 85, "coding": 90},
{"name": "Bob", "math": 70, "coding": 60},
{"name": "Charlie", "math": 95, "coding": 88},
{"name": "David", "math": 60, "coding": 55}
]

passed_students = [s['name'] for s in students if s['math'] >= 60 and s['coding'] >=60  ]
print(f"及格学生名单如下：{passed_students}"  )

avg_coding = sum(s['coding'] for s in students )/len(stduents)
print(f"coding平均分为：{avg_coding}")

df = pd.DataFrame(students)
df['total'] = df['math'] + df ['coding']
print(df)