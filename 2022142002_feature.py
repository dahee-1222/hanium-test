import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

directory = "C:\\3-1\\머신러닝 실습\\"

# 최대최소 이용한 정규화 함수 정의 : 데이터의 범위를 조정하여 일정한 범위
def normalize_data(input_feature): # 데이터의 범위 [0,1]
     min_vals = np.min(input_feature, axis=0) #열에서 최소값
     max_vals = np.max(input_feature, axis=0) #열에서 최대값
     selected_features_normalized = (input_feature - min_vals) / (max_vals - min_vals)
     return selected_features_normalized

# 특징 추출
def select_features(directory):
    
    path = directory + "heart_disease_new.csv"
    df = pd.read_csv(path)
   
    
    for column in df.select_dtypes(include='number').columns: #데이터의 값이 숫자인 열에서
      df[column] = df[column].fillna(df[column].mean()) # nan확인 및 평균으로 채우기(fillna를 이용해 nan인식)
    dataset = np.array(df) #데이터프레임을 넘파이 배열로 변환
    
    np.random.shuffle(dataset)
    
    # 선택한 특징 : BMI, Blood sugar levels, Heart rate, Cholesterol,Age
    
    "데이터 전처리"
   # 문자를 숫자로 바꾸기
   #np.where(조건,참일때의 값,거짓일때의 값)
    dataset[:, 0] = np.where(dataset[:, 0] == 'female', -1, 1)  #gender: female=-1, male=1
    dataset[:, 4] = np.where(dataset[:, 4] == 'no', -1, 1) #a neurological disorder: no=-1,yes=1
    dataset[:,14] = np.where(dataset[:,14] == 'no', 0, 1) #heart disease: no =0(2500개), yes = 1(500개)
   
    "특징 추출" #심근경색에 영향을 크게 미치는 요인들
    y_data = dataset[:,-1]
   
   #특징 1 #BMI + Blood sugar levels
    feature_1= dataset[:,2]+dataset[:,3] 
   #정규화
    feature_1 = normalize_data(feature_1)
   
   #특징 2  #Cholesterol + Age
    feature_2= dataset[:,8]+dataset[:,11]
    #정규화
    feature_2 = normalize_data(feature_2) 
    
   #특징 3  #Diabetes status + Smoking
    feature_3 = dataset[:,9]+dataset[:,12]
    #정규화
    feature_3 = normalize_data(feature_3)
    
   #특징 4 # a neurological disorder (뇌졸중)
    feature_4 = dataset[:,4]
    #정규화
    feature_4 = normalize_data(feature_4)
    
   #특징 합치기
    selected_features = np.column_stack((feature_1,feature_2,feature_3,feature_4))

   #특징 데이터 마지막 열에 y값 추가
    features = np.column_stack((selected_features, y_data))

    return features