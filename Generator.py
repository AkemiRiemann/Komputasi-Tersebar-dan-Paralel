import random

order, sortir = [], []

def generate_data(banyak_sortir, order_per_sortir):
    global order, sortir
    for _ in range(banyak_sortir):
        id_tempat = ''.join(random.choices('ABCDEFGHIJKLMNOPQRSTUVWXYZ', k=3)) + '-' + ''.join(random.choices('0123456789', k=4)) + '-' + ''.join(random.choices('ABCDEFGHIJKLMNOPQRSTUVWXYZ', k=3))
        nama_tempat = f"Sortir {_+1}"
        latitude = round(random.uniform(-90, 90), 6)
        longitude = round(random.uniform(-180, 180), 6)
        for p in range(order_per_sortir):
            id_order = ''.join(random.choices('ABCDEFGHIJKLMNOPQRSTUVWXYZ', k=3)) + '-' + ''.join(random.choices('0123456789', k=4)) + '-' + ''.join(random.choices('ABCDEFGHIJKLMNOPQRSTUVWXYZ', k=3))
            id_pemesan = ''.join(random.choices('ABCDEFGHIJKLMNOPQRSTUVWXYZ', k=3)) + '-' + ''.join(random.choices('0123456789', k=4)) + '-' + ''.join(random.choices('ABCDEFGHIJKLMNOPQRSTUVWXYZ', k=3))
            latitude_values = round(random.gauss(latitude, 5), 6)
            longitude_values = round(random.gauss(longitude, 5), 6)
            prioritas = random.randint(1, 3)
            order.append(f"{id_order}#{id_pemesan}#{latitude_values}#{longitude_values}#{prioritas}")

        sortir.append(f"{id_tempat}#{nama_tempat}#{latitude}#{longitude}")

def save_to_file(filename, data):
    with open(filename, 'w') as file:
        for entry in data:
            file.write(entry + '\n')

if __name__ == "__main__":
    banyak_sortir, order_per_sortir = 1000, 100
    data = generate_data(banyak_sortir, order_per_sortir)
    save_to_file('order.txt', order)
    save_to_file('sortir.txt', sortir)
    print(f"Berhasil membuat tempat sortir sebanyak {banyak_sortir} dan masing-masingnya memiliki {order_per_sortir} order")