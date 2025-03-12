import pandas as pd
import pathlib
import math 

# Đường dẫn đến tệp CSV
current_path = pathlib.Path(__file__).parent.absolute()
file_path = current_path / 'data' / 'solomon_dataset' / 'C1' / 'C101.csv'

# Đọc tệp CSV
df = pd.read_csv(file_path)

# Chuyển đổi DataFrame thành dictionary
data_dict = df.to_dict(orient='list')

# In ra dictionary
print(data_dict)

# In đường dẫn tệp CSV
print(file_path)

file_write_path = current_path / 'data' / 'solomon_dataset' / 'C1' / 'C101.txt'


CUST_NO = data_dict['CUST NO.']
X = data_dict['XCOORD.']
Y = data_dict['YCOORD.']
DEMAND = data_dict['DEMAND']
READY_TIME = data_dict['READY TIME']
DUE_DATE = data_dict['DUE DATE']
SERVICE_TIME = data_dict['SERVICE TIME']

with open(file_write_path, 'a', encoding='utf-8') as f:
    f.write(str(len(CUST_NO)))
    f.write('\n')
    f.write(str(max(DEMAND)))
    f.write('\n')

    for i in range(len(CUST_NO)+1):
        for j in range(len(CUST_NO)+1):
            fr = (0,0) if i == 0 else (X[i-1],Y[i-1])
            to = (0,0) if j == 0 else (X[j-1],Y[j-1])
            distance = ((fr[0]-to[0])**2 + (fr[1]-to[1])**2)**0.5
            f.write(f'{distance} ')
        f.write('\n')
    
    for i in range(len(CUST_NO)+1):
        if i ==0:
            f.write(f'0 {1e9}')
        else:
            f.write(f'{READY_TIME[i-1]} {DUE_DATE[i-1]}')
        f.write('\n')
    for i in range(len(CUST_NO)+1):
        if i ==0:
            f.write(f'0 ')
        else:
            f.write(f'{DEMAND[i-1]} ')
    f.write('\n')
    for i in range(len(CUST_NO)+1):
        if i ==0:
            f.write(f'0 ')
        else:
            f.write(f'{SERVICE_TIME[i-1]} ')
    f.write('\n')


        



