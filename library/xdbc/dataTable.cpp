#include "xdbc/dataTable.h"

__OPENCODE_BEGIN_NAMESPACE

DataTableModel::~DataTableModel() {}
DataTableModel::DataTableModel(){}

//--------------------------------------------------------------------------
// GenericTableModel class
//--------------------------------------------------------------------------

GenericTableModel::~GenericTableModel() {}
GenericTableModel::GenericTableModel(){}

bool GenericTableModel::useColumnIndicator(int column) const
{
  return false;
}

bool GenericTableModel::useColumnLength(int column) const
{
  return false;
}

bool GenericTableModel::useColumnCode(int column) const
{
  return false;
}


int GenericTableModel::getColumnIndicatorSize(int column) const
{
  return 0;
}

XLONG GenericTableModel::getColumnIndicatorOffset(int column) const
{
  return 0;
}

int GenericTableModel::getColumnIndicatorSkip(int column) const
{
  return 0;
}

int GenericTableModel::getColumnLengthSize(int column) const
{
  return 0;
}

XLONG GenericTableModel::getColumnLengthOffset(int column) const
{
  return 0;
}

int GenericTableModel::getColumnLengthSkip(int column) const
{
  return 0;
}


int GenericTableModel::getColumnCodeSize(int column) const
{
  return 0;
}

XLONG GenericTableModel::getColumnCodeOffset(int column) const
{
  return 0;
}

int GenericTableModel::getColumnCodeSkip(int column) const
{
  return 0;
}

const char*  GenericTableModel::getIndicatorBuffer() const
{
  return 0;
}

const char*  GenericTableModel::getLengthBuffer() const
{
  return 0;
}

const char*  GenericTableModel::getCodeBuffer() const
{
  return 0;
}

//--------------------------------------------------------------------------
// QueryTableModel class 
//--------------------------------------------------------------------------
QueryTableModel::~QueryTableModel()  { }

QueryTableModel::QueryTableModel(){}

bool QueryTableModel::useColumnIndicator(int column) const
{
  return false;
}

bool QueryTableModel::useColumnLength(int column) const
{
  return false;
}

bool QueryTableModel::useColumnCode(int column) const
{
  return false;
}

int QueryTableModel::getColumnIndicatorSize(int column) const
{
  return 0;
}

int QueryTableModel::getColumnLengthSize(int column) const
{
  return 0;
}

int QueryTableModel::getColumnCodeSize(int column) const
{
  return 0;
}

//--------------------------------------------------------------------------
// DataTable class
//--------------------------------------------------------------------------

DataTable::~DataTable() {}

DataTable::DataTable(const DataTableModel * aDataModel)
{
 setModel(aDataModel);
}

const DataTableModel* DataTable::getModel() const
{
  return dataModel;
}

void DataTable::setModel(const DataTableModel* aDataModel)
{
  if(aDataModel == 0)
  {
    return;
  }
  this->dataModel = aDataModel;
}

int DataTable::getRowCount() const
{
  return dataModel->getRowCount();
}

int DataTable::getColumnCount() const
{
  return dataModel->getColumnCount();
}

int DataTable::getColumnType(int column) const
{
  return dataModel->getColumnType(column);
}

XLONG DataTable::getColumnOffset(int column) const
{
  return dataModel->getColumnOffset(column);
}

int DataTable::getColumnSize(int column) const
{
  return dataModel->getColumnSize(column);
}

int  DataTable::getColumnSkip(int column) const
{
  return dataModel->getColumnSkip(column);
}


__OPENCODE_END_NAMESPACE

