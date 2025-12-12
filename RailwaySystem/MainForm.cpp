#include "MainForm.h"
#include "EditForm.h"
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
#include <string>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::IO;
using namespace RailwaySystem;
using namespace msclr::interop;

// Реализация вспомогательных методов преобразования строк
std::string MainForm::SystemStringToStdString(String^ str) {
    if (str == nullptr) return "";
    return marshal_as<std::string>(str);
}

String^ MainForm::StdStringToSystemString(const std::string& str) {
    return marshal_as<String^>(str);
}

// Конструктор
MainForm::MainForm(void) {
    InitializeComponent();
    station = new RailwayStation();

    // Добавление тестовых данных
    try {
        station->addRegularTariff("Саратов", 1500.0);
        station->addRegularTariff("Калининград", 2000.0);
        station->addDiscountTariff("Москва", 1200.0, 20.0);
        station->addDiscountTariff("Хельсинки", 2500.0, 25.0);
        UpdateDataGrid();
    }
    catch (const std::exception& e) {
        String^ message = marshal_as<String^>(e.what());
        MessageBox::Show(message, "Ошибка");
    }
}

// Деструктор
MainForm::~MainForm() {
    this->!MainForm();
    if (components) {
        delete components;
    }
}

// Финализатор
MainForm::!MainForm() {
    if (station != nullptr) {
        delete station;
        station = nullptr;
    }
}

// Инициализация компонентов формы
void MainForm::InitializeComponent(void) {
    this->components = gcnew System::ComponentModel::Container();

    // Создание DataGridView
    this->dataGridView1 = gcnew DataGridView();
    this->dataGridView1->Location = System::Drawing::Point(12, 50);
    this->dataGridView1->Size = System::Drawing::Size(760, 300);
    this->dataGridView1->TabIndex = 0;
    this->dataGridView1->ReadOnly = true;

    // Создание кнопок
    this->btnAdd = gcnew Button();
    this->btnAdd->Text = "Добавить";
    this->btnAdd->Location = System::Drawing::Point(12, 360);
    this->btnAdd->Size = System::Drawing::Size(120, 30);
    this->btnAdd->Click += gcnew System::EventHandler(this, &MainForm::btnAdd_Click);

    this->btnEdit = gcnew Button();
    this->btnEdit->Text = "Редактировать";
    this->btnEdit->Location = System::Drawing::Point(142, 360);
    this->btnEdit->Size = System::Drawing::Size(120, 30);
    this->btnEdit->Click += gcnew System::EventHandler(this, &MainForm::btnEdit_Click);

    this->btnDelete = gcnew Button();
    this->btnDelete->Text = "Удалить";
    this->btnDelete->Location = System::Drawing::Point(272, 360);
    this->btnDelete->Size = System::Drawing::Size(120, 30);
    this->btnDelete->Click += gcnew System::EventHandler(this, &MainForm::btnDelete_Click);

    this->btnLoad = gcnew Button();
    this->btnLoad->Text = "Загрузить";
    this->btnLoad->Location = System::Drawing::Point(12, 400);
    this->btnLoad->Size = System::Drawing::Size(120, 30);
    this->btnLoad->Click += gcnew System::EventHandler(this, &MainForm::btnLoad_Click);

    this->btnSave = gcnew Button();
    this->btnSave->Text = "Сохранить";
    this->btnSave->Location = System::Drawing::Point(142, 400);
    this->btnSave->Size = System::Drawing::Size(120, 30);
    this->btnSave->Click += gcnew System::EventHandler(this, &MainForm::btnSave_Click);

    this->btnSortDest = gcnew Button();
    this->btnSortDest->Text = "Сорт. по городу";
    this->btnSortDest->Location = System::Drawing::Point(272, 400);
    this->btnSortDest->Size = System::Drawing::Size(120, 30);
    this->btnSortDest->Click += gcnew System::EventHandler(this, &MainForm::btnSortDest_Click);

    this->btnSortPrice = gcnew Button();
    this->btnSortPrice->Text = "Сорт. по цене";
    this->btnSortPrice->Location = System::Drawing::Point(402, 400);
    this->btnSortPrice->Size = System::Drawing::Size(120, 30);
    this->btnSortPrice->Click += gcnew System::EventHandler(this, &MainForm::btnSortPrice_Click);

    this->btnStats = gcnew Button();
    this->btnStats->Text = "Статистика";
    this->btnStats->Location = System::Drawing::Point(532, 400);
    this->btnStats->Size = System::Drawing::Size(120, 30);
    this->btnStats->Click += gcnew System::EventHandler(this, &MainForm::btnStats_Click);

    this->btnFindMin = gcnew Button();
    this->btnFindMin->Text = "Мин. цена";
    this->btnFindMin->Location = System::Drawing::Point(652, 400);
    this->btnFindMin->Size = System::Drawing::Size(120, 30);
    this->btnFindMin->Click += gcnew System::EventHandler(this, &MainForm::btnFindMin_Click);

    // Диалоговые окна
    this->openFileDialog1 = gcnew OpenFileDialog();
    this->openFileDialog1->Filter = "Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*";

    this->saveFileDialog1 = gcnew SaveFileDialog();
    this->saveFileDialog1->Filter = "Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*";

    // Label
    this->label1 = gcnew Label();
    this->label1->Text = "Список тарифов:";
    this->label1->Location = System::Drawing::Point(12, 20);
    this->label1->Size = System::Drawing::Size(150, 20);

    // Настройка формы
    this->Text = "Управление тарифами железнодорожного вокзала";
    this->Size = System::Drawing::Size(800, 500);

    // Добавление элементов на форму
    this->Controls->Add(this->dataGridView1);
    this->Controls->Add(this->label1);
    this->Controls->Add(this->btnAdd);
    this->Controls->Add(this->btnEdit);
    this->Controls->Add(this->btnDelete);
    this->Controls->Add(this->btnLoad);
    this->Controls->Add(this->btnSave);
    this->Controls->Add(this->btnSortDest);
    this->Controls->Add(this->btnSortPrice);
    this->Controls->Add(this->btnStats);
    this->Controls->Add(this->btnFindMin);
}

// Обновление DataGridView
void MainForm::UpdateDataGrid() {
    dataGridView1->Rows->Clear();
    dataGridView1->Columns->Clear();

    // Создаем колонки
    dataGridView1->Columns->Add("Destination", "Направление");
    dataGridView1->Columns->Add("BasePrice", "Базовая цена");
    dataGridView1->Columns->Add("Discount", "Скидка (%)");
    dataGridView1->Columns->Add("FinalPrice", "Итоговая цена");
    dataGridView1->Columns->Add("Type", "Тип тарифа");

    for (size_t i = 0; i < station->getTariffCount(); ++i) {
        auto tariff = station->getTariff(i);
        double discount = 0.0;
        if (tariff->getTariffType() == "Со скидкой") {
            auto discountTariff = std::dynamic_pointer_cast<DiscountTariff>(tariff);
            if (discountTariff) {
                discount = discountTariff->getDiscountPercent();
            }
        }

        dataGridView1->Rows->Add(
            StdStringToSystemString(tariff->getDestination()),
            tariff->getBasePrice(),
            discount,
            tariff->getFinalPrice(),
            StdStringToSystemString(tariff->getTariffType())
        );
    }
}

// Обработчики событий
System::Void MainForm::btnAdd_Click(System::Object^ sender, System::EventArgs^ e) {
    EditForm^ editForm = gcnew EditForm();
    if (editForm->ShowDialog() == Windows::Forms::DialogResult::OK) {
        try {
            std::string destination = SystemStringToStdString(editForm->Destination);
            double price = editForm->Price;
            double discount = editForm->HasDiscount ? editForm->Discount : 0.0;

            if (discount > 0) {
                station->addDiscountTariff(destination, price, discount);
            }
            else {
                station->addRegularTariff(destination, price);
            }

            UpdateDataGrid();
            MessageBox::Show("Тариф успешно добавлен", "Успех");
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка");
        }
    }
}

System::Void MainForm::btnEdit_Click(System::Object^ sender, System::EventArgs^ e) {
    if (dataGridView1->SelectedRows->Count == 0) {
        MessageBox::Show("Выберите тариф для редактирования", "Ошибка");
        return;
    }

    int selectedIndex = dataGridView1->SelectedRows[0]->Index;
    if (selectedIndex < 0 || selectedIndex >= station->getTariffCount()) {
        MessageBox::Show("Неверный выбранный индекс", "Ошибка");
        return;
    }

    auto tariff = station->getTariff(selectedIndex);
    double discount = 0.0;
    if (tariff->getTariffType() == "Со скидкой") {
        auto discountTariff = std::dynamic_pointer_cast<DiscountTariff>(tariff);
        if (discountTariff) {
            discount = discountTariff->getDiscountPercent();
        }
    }

    EditForm^ editForm = gcnew EditForm(
        StdStringToSystemString(tariff->getDestination()),
        tariff->getBasePrice(),
        discount
    );

    if (editForm->ShowDialog() == Windows::Forms::DialogResult::OK) {
        try {
            std::string destination = SystemStringToStdString(editForm->Destination);
            double price = editForm->Price;
            double newDiscount = editForm->HasDiscount ? editForm->Discount : 0.0;

            station->updateTariff(selectedIndex, destination, price, newDiscount);
            UpdateDataGrid();
            MessageBox::Show("Тариф успешно обновлен", "Успех");
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка");
        }
    }
}

System::Void MainForm::btnDelete_Click(System::Object^ sender, System::EventArgs^ e) {
    if (dataGridView1->SelectedRows->Count == 0) {
        MessageBox::Show("Выберите тариф для удаления", "Ошибка");
        return;
    }

    int selectedIndex = dataGridView1->SelectedRows[0]->Index;
    if (selectedIndex < 0 || selectedIndex >= station->getTariffCount()) {
        MessageBox::Show("Неверный выбранный индекс", "Ошибка");
        return;
    }

    if (MessageBox::Show("Вы уверены, что хотите удалить выбранный тариф?",
        "Подтверждение", MessageBoxButtons::YesNo) == Windows::Forms::DialogResult::Yes) {
        try {
            station->removeTariff(selectedIndex);
            UpdateDataGrid();
            MessageBox::Show("Тариф успешно удален", "Успех");
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка");
        }
    }
}

System::Void MainForm::btnLoad_Click(System::Object^ sender, System::EventArgs^ e) {
    if (openFileDialog1->ShowDialog() == Windows::Forms::DialogResult::OK) {
        try {
            std::string filename = SystemStringToStdString(openFileDialog1->FileName);
            if (station->loadFromFile(filename)) {
                UpdateDataGrid();
                MessageBox::Show("Данные успешно загружены", "Успех");
            }
            else {
                MessageBox::Show("Ошибка при загрузке данных", "Ошибка");
            }
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка");
        }
    }
}

System::Void MainForm::btnSave_Click(System::Object^ sender, System::EventArgs^ e) {
    if (saveFileDialog1->ShowDialog() == Windows::Forms::DialogResult::OK) {
        try {
            std::string filename = SystemStringToStdString(saveFileDialog1->FileName);
            if (station->saveToFile(filename)) {
                MessageBox::Show("Данные успешно сохранены", "Успех");
            }
            else {
                MessageBox::Show("Ошибка при сохранении данных", "Ошибка");
            }
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка");
        }
    }
}

System::Void MainForm::btnSortDest_Click(System::Object^ sender, System::EventArgs^ e) {
    station->sortByDestination();
    UpdateDataGrid();
}

System::Void MainForm::btnSortPrice_Click(System::Object^ sender, System::EventArgs^ e) {
    station->sortByFinalPrice();
    UpdateDataGrid();
}

System::Void MainForm::btnStats_Click(System::Object^ sender, System::EventArgs^ e) {
    std::string stats = station->getStatistics();
    MessageBox::Show(StdStringToSystemString(stats), "Статистика");
}

System::Void MainForm::btnFindMin_Click(System::Object^ sender, System::EventArgs^ e) {
    try {
        auto minTariff = station->findMinCostDestination();
        std::string info = "Направление с минимальной стоимостью:\n" + minTariff->getInfoString();
        MessageBox::Show(StdStringToSystemString(info), "Минимальная цена");
    }
    catch (const std::exception& ex) {
        MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка");
    }
}