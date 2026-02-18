FROM nginx:alpine
COPY docker/nginx.conf /etc/nginx/conf.d/default.conf
WORKDIR /usr/share/nginx/html
# Core app
COPY index.html guest.html sw.js manifest.json llms.txt llms.html robots.txt sitemap.xml sitemap.txt ./
# Icons & images
COPY favicon-32.png apple-touch-icon.png icon-192.png icon-512.png logo.png og-image.png ./
# Docs & extras
COPY docs/ docs/
COPY botfaq.html stats.html ./
EXPOSE 80
