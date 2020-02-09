layersum=input('Please enter the total number of convolution layers:');
layersize=zeros(layersum,3); %  storage convolution layer parameters
min4=ones(layersum,1);
featuremapsize=zeros(layersum,1);
featuremapsize(1,1)=28;
featuremapsize(2,1)=10;
mul_sum=0;
max_layer=0;
P=ones(layersum,1);
AG=ones(layersum,1);  % maximum aG parameters for each layer
%peak_enery=input('Enter the peak energy in power trace:');

for layercnt=1:layersum % enter the size of each layer
    layersize(layercnt,1)=input(['Please enter the m value of the ',num2str(layercnt),' layer convolution core:']);
    layersize(layercnt,2)=input(['Please enter the n value of the ',num2str(layercnt),' layer convolution core:']);

    mul_mn=layersize(layercnt,1)*layersize(layercnt,2);
    if mul_mn>mul_sum
        mul_sum=mul_mn;
        max_layer=layercnt;
    end
    P(layercnt,1)=2.1*layersize(layercnt,1)+81.2*layersize(layercnt,2); % uW
    %     Power_peak=1e6*(0.0014*layersize(layercnt,1)+(0.1936+0.00156*min(2^ceil(sqrt(layersize(layercnt,1))),16))*layersize(layercnt,2))/800;
    %     AG(layercnt,1)=floor((peak_enery/2)/Power_peak);
    %     if AG(layercnt,1)==0
    %         AG(layercnt,1)=1;
    %     end
end

% AG parameter input
AG(1,1)=17;
AG(2,1)=4;
% AG(3,1)=3;
% AG(4,1)=3;
% AG(5,1)=5;

AGmax=max(AG);  % finding AG maximum
[AG_x,AG_y]=find(AG==AGmax);
Pmax=max(P);% finding the maximum Full size power consumption
tile=zeros(max(layersize(:,1))*7,7*layersum); % store all kinds of parameters of the convolution layer I to satisfy the requirement of partitioning

for layercnt=1:layersum % calculating tile scale parameters for each convolution layer
    cnt=1;
    for aG=1:AG(layercnt,1)
        Ptile=aG*(2.1*layersize(layercnt,1)+81.2*layersize(layercnt,2)); % uW
        Ttile=80; % ns
        tile(cnt,1+(layercnt-1)*7)=layersize(layercnt,1); %r
        tile(cnt,2+(layercnt-1)*7)=layersize(layercnt,1); %s
        tile(cnt,3+(layercnt-1)*7)=layersize(layercnt,2); %n
        tile(cnt,4+(layercnt-1)*7)=Ptile; %Power(uW) aG*1e6*Etile/Ttile
        tile(cnt,5+(layercnt-1)*7)=aG*layersize(layercnt,1)*layersize(layercnt,2)/Ttile; %Thoughout
        tile(cnt,6+(layercnt-1)*7)=aG;  % aG
        tile(cnt,7+(layercnt-1)*7)=Ttile;
        cnt=cnt+1;
    end
end

% get lower bound thresholds
lower_bound=tile(:,4+(AG_x-1)*7);
[m1,n1]=size(tile(:,4+(AG_x-1)*7));

lower_bound(all(lower_bound==0,2),:)=[];
lower_bound(all(lower_bound<Pmax,2),:)=[];
[m2,n2]=size(lower_bound);
lower_bound_2=ones(m2+2,1);
lower_bound_2(1,1)=0;
lower_bound_2(2,1)=Pmax;
for i=1:m2 
    lower_bound_2(i+2,1)=lower_bound(i,1);
end

out=ones(m2,3+layersum*5); % matrix written to excel

for i=1:m2 % row
    out(i,1)=i;
    
    if i==m2
        out(i,2)=lower_bound_2(i,1);
        out(i,3)=inf;
    else
        out(i,2)=lower_bound_2(i,1);
        out(i,3)=lower_bound_2(i+1,1);
    end
    
    for j=1:layersum % layer
        power_tem=0;
        for k=1:m1
            if tile(k,4+(j-1)*7)>=power_tem && tile(k,4+(j-1)*7)<out(i,3)
                power_tem=tile(k,4+(j-1)*7);
                out(i,j*5-1)=tile(k,1+(j-1)*7); % m
                out(i,j*5)=tile(k,3+(j-1)*7); % n
                out(i,j*5+1)=tile(k,6+(j-1)*7); % aG
                out(i,j*5+2)=tile(k,4+(j-1)*7);
                out(i,j*5+3)=80;
            end
        end
    end 
end

xlswrite('b.xls',out); % Write to excel file