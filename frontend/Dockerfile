FROM node:18-bullseye-slim

EXPOSE 3000

WORKDIR /app

COPY . .

RUN corepack enable
RUN pnpm install --frozen-lockfile
RUN pnpm build

RUN npm install -g serve

CMD ["serve", "-s", "dist", "-l", "3000"]
