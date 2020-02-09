layersum=input('Please enter the total number of convolution layers:');
layersize=zeros(layersum,3); %  storage convolution layer parameters
min4=ones(layersum,1);
featuremapsize=zeros(layersum,1);
featuremapsize(1,1)=28;
featuremapsize(2,1)=10;
mul_sum=0;
max_layer=0;
AG=ones(layersum,1);  % maximum aG parameters for each layer
%peak_enery=input('Enter the peak energy in power trace:');

for layercnt=1:layersum % enter the size of each layer
    layersize(layercnt,1)=input(['Please enter the m value of the ',num2str(layercnt),' layer convolution core:']);
    layersize(layercnt,2)=input(['Please enter the n value of the ',num2str(layercnt),' layer convolution core:']);
    
    mul_mn=layersize(layercnt,1)*layersize(layercnt,2);
    if mul_mn>mul_sum % Determining the Maximum Layer of Scale
        mul_sum=mul_mn;
        max_layer=layercnt; 
    end
    %     Power_peak=2.1*layersize(layercnt,1)+81.2*layersize(layercnt,2);
    %     AG(layercnt,1)=ceil((peak_enery/2)/Power_peak);
end

% AG parameter input
AG(1,1)=17;
AG(2,1)=4;
% AG(3,1)=3;
% AG(4,1)=3;
% AG(5,1)=5;

tile=zeros(max(layersize(:,1))*7,7*layersum); % store all kinds of parameters of the convolution layer I to satisfy the requirement of partitioning

for layercnt=1:layersum % calculating tile scale parameters for each convolution layer
    cnt=1;
    for i=1:layersize(layercnt,1)
        for k=1:layersize(layercnt,2)
            for aG=1:AG(layercnt,1)
                if mod(layersize(layercnt,1),i)==0&&mod(layersize(layercnt,2),k)==0
                    Ptile=aG*(2.1*i+81.2*k); % uW
                    Ttile=80; % ns
                    tile(cnt,1+(layercnt-1)*7)=i; %r
                    tile(cnt,2+(layercnt-1)*7)=i; %s
                    tile(cnt,3+(layercnt-1)*7)=k; %n
                    tile(cnt,4+(layercnt-1)*7)=Ptile; % Power(uW)
                    tile(cnt,5+(layercnt-1)*7)=1e9*aG*i*k/Ttile; %Thoughout
                    tile(cnt,6+(layercnt-1)*7)=aG;  % aG
                    tile(cnt,7+(layercnt-1)*7)=Ttile;
                    cnt=cnt+1;
                end
            end
        end
    end
    tile3min=tile; % output matrix
    tile3min(tile3min==0)=inf;
end

[m1,n1]=size(tile);

tile_sort=sortrows(tile3min,4+(max_layer-1)*7);
tile_sort(all(tile_sort==inf,2),:)=[];

thoughout_tem=0;
[m2,n2]=size(tile_sort);
out=ones(m2,3+layersum*5); % matrix written to excel

for i=1:m2 % row
    for j=1:layersum % layer
        for k=1:m2
            if tile_sort(k,5+(j-1)*7)>thoughout_tem && tile_sort(k,4+(j-1)*7)<=tile_sort(i,4+(max_layer-1)*7)
                power_tem=tile_sort(k,4+(j-1)*7);
                thoughout_tem=tile_sort(k,5+(j-1)*7);
                
                out(i,j*5-1)=tile_sort(k,1+(j-1)*7); % m
                out(i,j*5)=tile_sort(k,3+(j-1)*7); % n
                out(i,j*5+1)=tile_sort(k,6+(j-1)*7); % aG
                out(i,j*5+2)=tile_sort(k,4+(j-1)*7);
                out(i,j*5+3)=80;
            else if tile_sort(k,5+(j-1)*7)==thoughout_tem && tile_sort(k,4+(j-1)*7)<=tile_sort(i,4+(max_layer-1)*7)
                    if power_tem>tile_sort(k,4+(j-1)*7)
                        power_tem=tile_sort(k,4+(j-1)*7);
                        
                        out(i,j*5-1)=tile_sort(k,1+(j-1)*7); % m
                        out(i,j*5)=tile_sort(k,3+(j-1)*7); % n
                        out(i,j*5+1)=tile_sort(k,6+(j-1)*7); % aG
                        out(i,j*5+2)=tile_sort(k,4+(j-1)*7);
                        out(i,j*5+3)=80;
                    end
                end
            end
        end
    end
    out(i,1)=i;
    if i==m2
        out(i,2)=tile_sort(i,4+(max_layer-1)*7);
        out(i,3)=inf;
    else
        out(i,2)=tile_sort(i,4+(max_layer-1)*7);
        out(i,3)=tile_sort(i+1,4+(max_layer-1)*7);
    end
    
    power_tem=0;
    thoughout_tem=0;
    
end

out_merge=ones(m2,3+layersum*5); % merge matrix
out_merge(1,:)=out(1,:);
cnt=2;

for i=2:m2
    for j=1:layersum % layer
        if out(i,j*5+2)~=out(i-1,j*5+2)
             out_merge(cnt,:)=out(i,:);
             out_merge(cnt,1)=cnt;
             out_merge(cnt,2)=out_merge(cnt-1,3);
             cnt=cnt+1;
        end
    end
end
out_merge(all(out_merge==1,2),:)=[]; % eliminate redundant rows

xlswrite('c.xls',out); % Write to excel file