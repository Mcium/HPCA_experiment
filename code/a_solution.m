layersum=input('Please enter the total number of convolution layers:');
layersize=zeros(layersum,3); %  storage convolution layer parameters
P=ones(layersum,1);

for layercnt=1:layersum % enter the size of each layer
    layersize(layercnt,1)=input(['Please enter the m value of the ',num2str(layercnt),' layer convolution core:']);
    layersize(layercnt,2)=input(['Please enter the n value of the ',num2str(layercnt),' layer convolution core:']);
 
    P(layercnt,1)=2.1*layersize(layercnt,1)+81.2*layersize(layercnt,2); % uW
end

Pmax=max(P); % Finding the Maximum Full Size Power Consumption
out=cell(2,3+layersum*5); % Matrix written to excel

for i=1:layersum % Write data to each layer
    out{2,i*5-1}=num2str(layersize(i,1)); %m
    out{2,i*5}=num2str(layersize(i,2)); %n
    out{2,i*5+1}=num2str(1); %aG=1
    out{2,i*5+2}=num2str(P(i,1)); %P
    out{2,i*5+3}=80; %latency
end

for i=4:3+layersum*5
    out{1,i}=num2str(0);
end

out{1,1}=num2str(1);
out{2,1}=num2str(2);
out{1,2}=num2str(0);
out{2,2}=num2str(Pmax);
out{1,3}=num2str(Pmax);
out{2,3}=num2str(Inf);

xlswrite('a.xls',out,1,'A2'); % Write to excel file