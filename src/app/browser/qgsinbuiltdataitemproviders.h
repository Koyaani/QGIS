/***************************************************************************
                         qgsinbuiltdataitemproviders.h
                         --------------------------
    begin                : October 2018
    copyright            : (C) 2018 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSINBUILTDATAITEMPROVIDERS_H
#define QGSINBUILTDATAITEMPROVIDERS_H

#include "qgis_app.h"
#include "qgsdataitemguiprovider.h"
#include <QObject>

class QgsDirectoryItem;
class QgsFavoriteItem;
class QgsLayerItem;
class QgsFieldsItem;
class QgsFieldItem;

class QgsAppDirectoryItemGuiProvider : public QObject, public QgsDataItemGuiProvider
{
    Q_OBJECT

  public:

    QgsAppDirectoryItemGuiProvider() = default;

    QString name() override;

    void populateContextMenu( QgsDataItem *item, QMenu *menu,
                              const QList<QgsDataItem *> &selectedItems, QgsDataItemGuiContext context ) override;

  private:

    static void addFavorite( QgsDirectoryItem *item );
    static void removeFavorite( QgsFavoriteItem *favorite );
    static void renameFavorite( QgsFavoriteItem *favorite );
    static void changeDirectoryColor( QgsDirectoryItem *item );
    static void clearDirectoryColor( QgsDirectoryItem *item );
    static void hideDirectory( QgsDirectoryItem *item );
    static void toggleFastScan( QgsDirectoryItem *item );
    static void toggleMonitor( QgsDirectoryItem *item );
    static void showProperties( QgsDirectoryItem *item, QgsDataItemGuiContext context );
};

class QgsAppFileItemGuiProvider : public QObject, public QgsDataItemGuiProvider
{
    Q_OBJECT

  public:

    QgsAppFileItemGuiProvider() = default;

    QString name() override;

    void populateContextMenu( QgsDataItem *item, QMenu *menu,
                              const QList<QgsDataItem *> &selectedItems, QgsDataItemGuiContext context ) override;
    int precedenceWhenPopulatingMenus() const override;
    bool rename( QgsDataItem *item, const QString &name, QgsDataItemGuiContext context ) override;

  private:

    static bool rename( const QString &oldPath, const QString &newName, QgsDataItemGuiContext context, const QList< QPointer< QgsDataItem > > &parentItems );
};


class QgsProjectHomeItemGuiProvider : public QObject, public QgsDataItemGuiProvider
{
    Q_OBJECT

  public:

    QgsProjectHomeItemGuiProvider() = default;

    QString name() override;

    void populateContextMenu( QgsDataItem *item, QMenu *menu,
                              const QList<QgsDataItem *> &selectedItems, QgsDataItemGuiContext context ) override;

};


class QgsFavoritesItemGuiProvider : public QObject, public QgsDataItemGuiProvider
{
    Q_OBJECT

  public:

    QgsFavoritesItemGuiProvider() = default;

    QString name() override;

    void populateContextMenu( QgsDataItem *item, QMenu *menu,
                              const QList<QgsDataItem *> &selectedItems, QgsDataItemGuiContext context ) override;

};


class QgsLayerItemGuiProvider : public QObject, public QgsDataItemGuiProvider
{
    Q_OBJECT

  public:

    QgsLayerItemGuiProvider() = default;

    QString name() override;

    void populateContextMenu( QgsDataItem *item, QMenu *menu,
                              const QList<QgsDataItem *> &selectedItems, QgsDataItemGuiContext context ) override;
    int precedenceWhenPopulatingMenus() const override;
    bool handleDoubleClick( QgsDataItem *item, QgsDataItemGuiContext context ) override;

  private:

    static void addLayersFromItems( const QList<QgsDataItem *> &items );
    static void showPropertiesForItem( QgsLayerItem *item, QgsDataItemGuiContext context );
    static void deleteLayers( const QStringList &itemPath, QgsDataItemGuiContext context );

};


class QgsFieldsItemGuiProvider : public QObject, public QgsDataItemGuiProvider
{
    Q_OBJECT

  public:

    QgsFieldsItemGuiProvider() = default;

    QString name() override;

    void populateContextMenu( QgsDataItem *item, QMenu *menu,
                              const QList<QgsDataItem *> &selectedItems, QgsDataItemGuiContext context ) override;

};


class QgsFieldItemGuiProvider : public QObject, public QgsDataItemGuiProvider
{
    Q_OBJECT

  public:

    QgsFieldItemGuiProvider() = default;

    QString name() override;

    void populateContextMenu( QgsDataItem *item, QMenu *menu,
                              const QList<QgsDataItem *> &selectedItems, QgsDataItemGuiContext context ) override;

};


class QgsDatabaseItemGuiProvider : public QObject, public QgsDataItemGuiProvider
{
    Q_OBJECT

  public:

    QgsDatabaseItemGuiProvider() = default;

    QString name() override;

    void populateContextMenu( QgsDataItem *item, QMenu *menu,
                              const QList<QgsDataItem *> &selectedItems, QgsDataItemGuiContext context ) override;

};



class QgsProjectItemGuiProvider : public QObject, public QgsDataItemGuiProvider
{
    Q_OBJECT

  public:

    QgsProjectItemGuiProvider() = default;

    QString name() override;

    void populateContextMenu( QgsDataItem *item, QMenu *menu,
                              const QList<QgsDataItem *> &selectedItems, QgsDataItemGuiContext context ) override;
    bool handleDoubleClick( QgsDataItem *item, QgsDataItemGuiContext context ) override;

};

#endif // QGSINBUILTDATAITEMPROVIDERS_H


