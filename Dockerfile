# GCC image
FROM gcc:4.9

# Directorio de trabajo
WORKDIR /app

# Copio archivos de la carpeta actual al contenedor
COPY . .

# Compilo Main.cpp
RUN g++ -o Test ./Sockets.Server/Main.cpp

# Lo corro porque si
CMD ["./Test"]

# Correr el "docker run <name> -p "puerto:puerto" -d" - > docker run socket-test -p "3000:3000" -d