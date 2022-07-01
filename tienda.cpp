#include "tienda.h"
#include "ui_tienda.h"

#include <QDebug>
#include <QMessageBox>
#include <QDebug>
#include <QWidget>
#include <QFile>

Tienda::Tienda(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Tienda)
{
    QFile archivo;
    QTextStream io;
    QString nombreArchivo="/Applications/Productos.csv";
    int cod;
    QString pro;
    float pre;
    archivo.setFileName(nombreArchivo);
    archivo.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!archivo.isOpen()){
        QMessageBox::critical(this,"Error",archivo.errorString());
        return;
    }
    //Usuario, Password
    io.setDevice(&archivo);
    while(!io.atEnd()){
        auto linea = io.readLine();
        auto datos = linea.split(";");
        qDebug() << datos;
        cod = datos[0].toInt();
        pro = datos[1];
        pre = datos[2].toFloat();
        m_productos.append(new Producto(cod,pro,pre));
    }

    archivo.close();
    ui->setupUi(this);
    // Lista de productos
    cargarProductos();
    // Mostrar los productos en el combo
    foreach (Producto *p, m_productos){
        ui->inProducto->addItem(p->nombre());
    }
    // Configurar cabecera de la tabla
    QStringList cabecera = {"Cantidad", "Producto", "P. unitario", "Subtotal"};
    ui->outDetalle->setColumnCount(4);
    ui->outDetalle->setHorizontalHeaderLabels(cabecera);
    // Establecer el subtotal a 0
    m_subtotal = 0;
}

Tienda::~Tienda()
{
    delete ui;
}
/**
 * @brief Tienda::cargarProductos Carga la lista de productos de la tienda
 */
void Tienda::cargarProductos()
{
    // Crear productos "quemados" en el código
    m_productos.append(new Producto(1, "Leche", 0.80));
    m_productos.append(new Producto(2, "Pan", 0.15));
    m_productos.append(new Producto(3, "Queso", 2.50));
    // Podría leerse de una base de datos, de un archivo o incluso de Internet
}

void Tienda::calcular(float stProducto)
{
    // Calcular valores
    m_subtotal += stProducto;
    float iva = m_subtotal * IVA / 100;
    float total = m_subtotal + iva;
    // Mostrar valores en GUI
    ui->outSubtotal->setText("$ " + QString::number(m_subtotal, 'f', 2));
    ui->outIva->setText("$ " + QString::number(iva, 'f', 2));
    ui->outTotal->setText("$ " + QString::number(total, 'f', 2));
}


void Tienda::on_inProducto_currentIndexChanged(int index)
{
    // Obtener el precio del producto actual seleccionado
    float precio = m_productos.at(index)->precio();
    // Mostrar el precio del product en la etiqueta
    ui->outPrecio->setText("$ " + QString::number(precio,'f',2));
    // Resetear el spinbox de cantidad
    ui->inCantidad->setValue(0);
}


void Tienda::on_btnAgregar_released()
{
    // Validar que no se agregen productos cpn 0 cantidad
    int cantidad = ui->inCantidad->value();

    if (cantidad == 0){
        return;
    }
    // Obtener los datos de la GUI
    int i = ui->inProducto->currentIndex();
    Producto *p = m_productos.at(i);
    float subtotal=p->precio()*cantidad;
    // Calcular el subrotal del producto


    // Agregar los datos a la tabla
    int fila = ui->outDetalle->rowCount();
    CantidadExistente=0.0;
    filaEncontrada=0;
    if (buscarproducto(p->nombre(),fila,cantidad)){
        float cantidadAnterior=p->precio()*CantidadExistente;
        subtotal=p->precio()*CantidadActual;
        ui->outDetalle->setFocus();
        ui->outDetalle->setItem(filaEncontrada, 0, new QTableWidgetItem(QString::number(CantidadActual)));
        ui->outDetalle->setItem(filaEncontrada, 3, new QTableWidgetItem(QString::number(subtotal,'f',2)));
        m_subtotal-=cantidadAnterior;
    }else{
        ui->outDetalle->insertRow(fila);
        ui->outDetalle->setItem(fila, 0, new QTableWidgetItem(QString::number(cantidad)));
        ui->outDetalle->setItem(fila, 1, new QTableWidgetItem(p->nombre()));
        ui->outDetalle->setItem(fila, 2, new QTableWidgetItem(QString::number(p->precio(),'f',2)));
        ui->outDetalle->setItem(fila, 3, new QTableWidgetItem(QString::number(subtotal,'f',2)));
    }

    // Limpiar datos
    ui->inCantidad->setValue(0);
    ui->inProducto->setFocus();

    // Actualizar subtotales
    calcular(subtotal);

}

bool Tienda::buscarproducto(QString nombreProducto, int n, int cantidad)
{
    QString celda;
    bool bandera=false;
    for(int i=0;i<n;i++){//filas
         celda=ui->outDetalle->item(i,1)->text();
         if(nombreProducto==celda){
             bandera=true;
             filaEncontrada=i;
             CantidadExistente=ui->outDetalle->item(i,0)->text().toFloat();
             CantidadActual=+cantidad+CantidadExistente;
           }
    }
    return bandera;
}

void Tienda::on_pushButton_released()
{
    Factura *Dato = new Factura(this);
}

