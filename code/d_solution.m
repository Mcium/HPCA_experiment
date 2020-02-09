layersum=input('Please enter the total number of convolution layers:');
layersize=zeros(layersum,3); %  storage convolution layer parameters
min4=ones(layersum,1);
featuremapsize=zeros(layersum,1);
featuremapsize(1,1)=28;
featuremapsize(2,1)=10;
mul_sum=0;
max_layer=0;
AG=ones(layersum,1);  % maximum aG parameters for each layer

for layercnt=1:layersum % enter the size of each layer
    layersize(layercnt,1)=input(['Please enter the m value of the ',num2str(layercnt),' layer convolution core:']);
    layersize(layercnt,2)=input(['Please enter the n value of the ',num2str(layercnt),' layer convolution core:']);
    
    mul_mn=layersize(layercnt,1)*layersize(layercnt,2);
    if mul_mn>mul_sum
        mul_sum=mul_mn;
        max_layer=layercnt;
    end
    %     Power_peak=0.0011*layersize(layercnt,1)+79.26*layersize(layercnt,2);
    %     AG(layercnt,1)=ceil((peak_enery/2)/Power_peak);
end

% AG parameter input

AG(1,1)=17;
AG(2,1)=4;
% AG(3,1)=3;
% AG(4,1)=3;
% AG(5,1)=5;

tile=zeros(max(layersize(:,1))*7,7*layersum); % store all kinds of parameters of the convolution layer I to satisfy the requirement of partitioning
occurdata=zeros(700,layersum);

for layercnt=1:layersum % calculating tile scale parameters for each convolution layer
    cnt=1;
    for i=1:layersize(layercnt,1)
        for j=1:layersize(layercnt,1)
            for k=1:layersize(layercnt,2)
                for aG=1:AG(layercnt,1)
                    if mod(layersize(layercnt,1),i*j)==0&&mod(layersize(layercnt,2),k)==0
                        Ptile=aG*(2.1*i*j+81.2*k); % uW
                        Ttile=80; % 100+37.5*min(2^ceil(sqrt(i*j)),16);
                        Retile=roundn(Ptile,-2);
                        if ~any(occurdata(:,layercnt)==Retile) % judging whether to repeat
                            tile(cnt,1+(layercnt-1)*7)=i; %r
                            tile(cnt,2+(layercnt-1)*7)=j; %s
                            tile(cnt,3+(layercnt-1)*7)=k; %n
                            tile(cnt,4+(layercnt-1)*7)=Ptile; %Power(uW)
                            tile(cnt,5+(layercnt-1)*7)=aG*1e9*i*j*k/Ttile; %Thoughout
                            tile(cnt,6+(layercnt-1)*7)=aG;  % aG
                            tile(cnt,7+(layercnt-1)*7)=Ttile;
                            occurdata(cnt,layercnt)=Retile;
                            cnt=cnt+1;
                        end
                    end
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

[m2,n2]=size(tile_sort);
out=ones(m2,4+layersum*5); % matrix written to excel
Power_pipeline=zeros(m2,1);

for i=1:m2 % row
    for j=1:layersum % layer
        if j==max_layer
            out(i,j*5-1)=tile_sort(i,2+(j-1)*7);
            out(i,j*5)=tile_sort(i,3+(j-1)*7);
            out(i,j*5+1)=tile_sort(i,6+(j-1)*7);
            out(i,j*5+2)=tile_sort(i,4+(j-1)*7);
            out(i,j*5+3)=80;
        else
            for k=1:m2
                if tile_sort(k,5+(j-1)*7)>thoughout_tem && tile_sort(k,4+(j-1)*7)<=tile_sort(i,4+(max_layer-1)*7)
                    power_tem=tile_sort(k,4+(j-1)*7);
                    thoughout_tem=tile_sort(k,5+(j-1)*7);
                    out(i,j*5-1)=tile_sort(k,2+(j-1)*7);
                    out(i,j*5)=tile_sort(k,3+(j-1)*7);
                    out(i,j*5+1)=tile_sort(k,6+(j-1)*7);
                    out(i,j*5+2)=tile_sort(k,4+(j-1)*7);
                    out(i,j*5+3)=80;
                else if tile_sort(k,5+(j-1)*7)==thoughout_tem && tile_sort(k,4+(j-1)*7)<=tile_sort(i,4+(max_layer-1)*7)
                        if power_tem>tile_sort(k,4+(j-1)*7)
                            power_tem=tile_sort(k,4+(j-1)*7);
                            out(i,j*5-1)=tile_sort(k,1+(j-1)*7);
                            out(i,j*5)=tile_sort(k,3+(j-1)*7);
                            out(i,j*5+1)=tile_sort(k,6+(j-1)*7);
                            out(i,j*5+2)=tile_sort(k,4+(j-1)*7);
                            out(i,j*5+3)=80;
                        end
                    end
                end
            end
        end
        Power_pipeline(i,1)=Power_pipeline(i,1)+out(i,j*5+2);
    end
    out(i,1)=i;
    out(i,4+layersum*5)=Power_pipeline(i,1); % Power-pipeline
    power_tem=0;
    thoughout_tem=0;
end

out_sort=sortrows(out,4+layersum*5);
for i=1:m2
    out_sort(i,1)=i;
    if i==m2
        out_sort(i,2)= out_sort(i,4+layersum*5);
    else
        out_sort(i,2)=out_sort(i,4+layersum*5);
        out_sort(i,3)=out_sort(i+1,4+layersum*5);
    end
end

for i=1:m2 % traversing through each convolution layer to find the optimal solution
    for j=1:layersum % layer
        thoughout_tem=0;
        power_tem=0;
        lim=out_sort(i,2+j*5);
        for k=1:m2
            if tile_sort(k,5+(j-1)*7)>thoughout_tem && tile_sort(k,4+(j-1)*7)<=lim
                power_tem=tile_sort(k,4+(j-1)*7);
                thoughout_tem=tile_sort(k,5+(j-1)*7);
                
                out_sort(i,j*5-1)=tile_sort(k,2+(j-1)*7);
                out_sort(i,j*5)=tile_sort(k,3+(j-1)*7);
                out_sort(i,j*5+1)=tile_sort(k,6+(j-1)*7);
                out_sort(i,j*5+2)=tile_sort(k,4+(j-1)*7);
                out_sort(i,j*5+3)=80;
            else if tile_sort(k,5+(j-1)*7)==thoughout_tem && tile_sort(k,4+(j-1)*7)<=lim
                    if power_tem>tile_sort(k,4+(j-1)*7)
                        power_tem=tile_sort(k,4+(j-1)*7);
                        out_sort(i,j*5-1)=tile_sort(k,2+(j-1)*7);
                        out_sort(i,j*5)=tile_sort(k,3+(j-1)*7);
                        out_sort(i,j*5+1)=tile_sort(k,6+(j-1)*7);
                        out_sort(i,j*5+2)=tile_sort(k,4+(j-1)*7);
                        out_sort(i,j*5+3)=80;
                    end
                end
            end
        end
    end
end
out_merge=ones(m2,4+layersum*5); % merge matrix
out_merge(1,:)=out_sort(1,:);
cnt=2;

for i=2:m2
    for j=1:layersum % layer
        if out_sort(i,j*5+2)~=out_sort(i-1,j*5+2)
            out_merge(cnt,:)=out_sort(i,:);
            out_merge(cnt,1)=cnt;
            out_merge(cnt,2)=out_merge(cnt-1,3);
            cnt=cnt+1;
        end
    end
end
out_merge(all(out_merge==1,2),:)=[]; % eliminate redundant rows

xlswrite('d.xls',out_merge,1,'A2'); % Write to excel file