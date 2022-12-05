FROM node
WORKDIR /react-app
COPY ./package.json /react-app
RUN npm install --legacy-peer-deps
COPY . .
EXPOSE 3000
CMD npm start