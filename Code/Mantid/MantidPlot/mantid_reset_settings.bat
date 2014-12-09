:: Copyright &copy; 2007-2014 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

:: This file is part of Mantid.

:: Mantid is free software; you can redistribute it and/or modify
:: it under the terms of the GNU General Public License as published by
:: the Free Software Foundation; either version 3 of the License, or
:: (at your option) any later version.

:: Mantid is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
:: GNU General Public License for more details.

:: You should have received a copy of the GNU General Public License
:: along with this program.  If not, see <http://www.gnu.org/licenses/>.

:: File change history is stored at: <https://github.com/mantidproject/mantid>.
:: Code Documentation is available at: <http://doxygen.mantidproject.org>

@echo off

set pref_file=Mantid.user.properties

if exist %pref_file%. del %pref_file%. else echo %pref_file% not found

python.exe %~dp0\mantid_qt_settings_editor.py -fc
