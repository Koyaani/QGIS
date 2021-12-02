/***************************************************************************
     testqgsgeometrycollection.cpp
     --------------------------------------
    Date                 : August 2021
    Copyright            : (C) 2021 by Loïc Bartoletti
    Email                : loic dot bartoletti at oslandia dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qgstest.h"
#include <QObject>
#include <QString>

#include "qgscircularstring.h"
#include "qgsgeometrycollection.h"
#include "qgsgeometryutils.h"
#include "qgslinestring.h"
#include "qgsmultilinestring.h"
#include "qgsmultipoint.h"
#include "qgspoint.h"
#include "qgspolygon.h"
#include "qgsproject.h"
#include "qgscoordinatetransform.h"
#include "testgeometryutils.h"
#include "testtransformer.h"

class TestQgsGeometryCollection: public QObject
{
    Q_OBJECT
  private slots:
    void constructor();
    void addBadGeometry();
    void addGeometry();
    void addGeometryInitialDimension();
    void insertGeometry();
    void removeGeometry();
    void assignment();
    void copy();
    void clone();
    void clear();
    void equality();
    void swapXy();
    void dropZValue();
    void dropMValue();
    void closestSegment();
    void hasCurvedSegments();
    void boundary();
    void segmentize();
    void cast();
    void insertVertex();
    void moveVertex();
    void deleteVertex();
    void vertexNumberFromVertexId();
    void nextVertex();
    void adjacentVertices();
    void vertexAngle();
    void removeDuplicateNodes();
    void filterVertices();
    void transformVertices();
    void transformWithClass();
    void transform();
    void qTransform();
    void toCurveType();
    void toFromWKB();
    void toFromWKT();
    void exportImport();
};

void TestQgsGeometryCollection::constructor()
{
  QgsGeometryCollection gc;

  QVERIFY( gc.isEmpty() );
  QCOMPARE( gc.nCoordinates(), 0 );
  QCOMPARE( gc.ringCount(), 0 );
  QCOMPARE( gc.partCount(), 0 );
  QVERIFY( !gc.is3D() );
  QVERIFY( !gc.isMeasure() );
  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
  QCOMPARE( gc.wktTypeStr(), QString( "GeometryCollection" ) );
  QCOMPARE( gc.geometryType(), QString( "GeometryCollection" ) );
  QCOMPARE( gc.dimension(), 0 );
  QVERIFY( !gc.hasCurvedSegments() );
  QCOMPARE( gc.area(), 0.0 );
  QCOMPARE( gc.perimeter(), 0.0 );
  QCOMPARE( gc.numGeometries(), 0 );
  QVERIFY( !gc.geometryN( 0 ) );
  QVERIFY( !gc.geometryN( -1 ) );
  QCOMPARE( gc.vertexCount( 0, 0 ), 0 );
  QCOMPARE( gc.vertexCount( 0, 1 ), 0 );
  QCOMPARE( gc.vertexCount( 1, 0 ), 0 );
}

void TestQgsGeometryCollection::addBadGeometry()
{
  QgsGeometryCollection gc;

  //try with nullptr
  gc.addGeometry( nullptr );
  QVERIFY( gc.isEmpty() );
  QCOMPARE( gc.nCoordinates(), 0 );
  QCOMPARE( gc.ringCount(), 0 );
  QCOMPARE( gc.partCount(), 0 );
  QCOMPARE( gc.numGeometries(), 0 );
  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
  QVERIFY( !gc.geometryN( 0 ) );
  QVERIFY( !gc.geometryN( -1 ) );
}

void TestQgsGeometryCollection::addGeometry()
{
  QgsGeometryCollection gc;

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 0, 0 )
                  << QgsPoint( 0, 10 ) << QgsPoint( 10, 10 )
                  << QgsPoint( 10, 0 ) << QgsPoint( 0, 0 ) );
  gc.addGeometry( part.clone() );

  QVERIFY( !gc.isEmpty() );
  QCOMPARE( gc.numGeometries(), 1 );
  QCOMPARE( gc.nCoordinates(), 5 );
  QCOMPARE( gc.ringCount(), 1 );
  QCOMPARE( gc.partCount(), 1 );
  QVERIFY( !gc.is3D() );
  QVERIFY( !gc.isMeasure() );
  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
  QCOMPARE( gc.wktTypeStr(), QString( "GeometryCollection" ) );
  QCOMPARE( gc.geometryType(), QString( "GeometryCollection" ) );
  QCOMPARE( gc.dimension(), 1 );
  QVERIFY( !gc.hasCurvedSegments() );
  QCOMPARE( gc.area(), 0.0 );
  QCOMPARE( gc.perimeter(), 0.0 );
  QVERIFY( gc.geometryN( 0 ) );
  QVERIFY( !gc.geometryN( 100 ) );
  QVERIFY( !gc.geometryN( -1 ) );
  QCOMPARE( gc.vertexCount( 0, 0 ), 5 );
  QCOMPARE( gc.vertexCount( 1, 0 ), 0 );

  //retrieve geometry and check
  QCOMPARE( *( static_cast< const QgsLineString * >( gc.geometryN( 0 ) ) ), part );

  //add another part
  gc.clear();

  part.setPoints( QgsPointSequence() << QgsPoint( 0, 0 )
                  << QgsPoint( 0, 10 ) << QgsPoint( 10, 10 )
                  << QgsPoint( 10, 0 ) << QgsPoint( 0, 0 ) );
  gc.addGeometry( part.clone() );

  QCOMPARE( gc.vertexCount( 0, 0 ), 5 );

  part.setPoints( QgsPointSequence() << QgsPoint( 1, 1 )
                  << QgsPoint( 1, 9 ) << QgsPoint( 9, 9 )
                  << QgsPoint( 9, 1 ) << QgsPoint( 1, 1 ) );
  gc.addGeometry( part.clone() );

  QCOMPARE( gc.vertexCount( 1, 0 ), 5 );
  QCOMPARE( gc.numGeometries(), 2 );
  QVERIFY( gc.geometryN( 0 ) );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 1 ) ), part );

  QgsCoordinateSequence seq = gc.coordinateSequence();
  QgsCoordinateSequence expectedSeq;
  expectedSeq << ( QgsRingSequence() << ( QgsPointSequence() << QgsPoint( 0, 0 )
                                          << QgsPoint( 0, 10 ) << QgsPoint( 10, 10 )
                                          << QgsPoint( 10, 0 ) << QgsPoint( 0, 0 ) ) );
  expectedSeq << ( QgsRingSequence() << ( QgsPointSequence() << QgsPoint( 1, 1 )
                                          << QgsPoint( 1, 9 ) << QgsPoint( 9, 9 )
                                          << QgsPoint( 9, 1 ) << QgsPoint( 1, 1 ) ) );

  QCOMPARE( seq, expectedSeq );
  QCOMPARE( gc.nCoordinates(), 10 );
}

void TestQgsGeometryCollection::addGeometryInitialDimension()
{
  QgsGeometryCollection gc;

  //initial adding of geometry should set z/m type
  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZ, 0, 0, 1 )
                  << QgsPoint( QgsWkbTypes::PointZ, 0, 10, 2 )
                  << QgsPoint( QgsWkbTypes::PointZ, 10, 10, 3 )
                  << QgsPoint( QgsWkbTypes::PointZ, 10, 0, 4 )
                  << QgsPoint( QgsWkbTypes::PointZ, 0, 0, 1 ) );
  gc.addGeometry( part.clone() );

  //QVERIFY( gc.is3D() ); //no meaning for collections?
  //QVERIFY( !gc.isMeasure() ); //no meaning for collections?
  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
  QCOMPARE( gc.wktTypeStr(), QString( "GeometryCollection" ) );
  QCOMPARE( gc.geometryType(), QString( "GeometryCollection" ) );
  QCOMPARE( *( static_cast< const QgsLineString * >( gc.geometryN( 0 ) ) ), part );

  gc.clear();
  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointM, 0, 0, 0, 1 )
                  << QgsPoint( QgsWkbTypes::PointM, 0, 10, 0, 2 )
                  << QgsPoint( QgsWkbTypes::PointM, 10, 10, 0, 3 )
                  << QgsPoint( QgsWkbTypes::PointM, 10, 0, 0, 4 )
                  << QgsPoint( QgsWkbTypes::PointM, 0, 0, 0, 1 ) );
  gc.addGeometry( part.clone() );

  //QVERIFY( !gc.is3D() ); //no meaning for collections?
  //QVERIFY( gc.isMeasure() ); //no meaning for collections?
  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
  QCOMPARE( gc.wktTypeStr(), QString( "GeometryCollection" ) );
  QCOMPARE( *( static_cast< const QgsLineString * >( gc.geometryN( 0 ) ) ), part );

  gc.clear();
  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 2, 1 )
                  << QgsPoint( QgsWkbTypes::PointZM, 0, 10, 3, 2 )
                  << QgsPoint( QgsWkbTypes::PointZM, 10, 10, 5, 3 )
                  << QgsPoint( QgsWkbTypes::PointZM, 10, 0, 0, 4 )
                  << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 2, 1 ) );
  gc.addGeometry( part.clone() );

  //QVERIFY( gc.is3D() ); //no meaning for collections?
  //QVERIFY( gc.isMeasure() ); //no meaning for collections?
  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
  QCOMPARE( gc.wktTypeStr(), QString( "GeometryCollection" ) );
  QCOMPARE( *( static_cast< const QgsLineString * >( gc.geometryN( 0 ) ) ), part );
}

void TestQgsGeometryCollection::insertGeometry()
{
  QgsGeometryCollection gc;

  gc.insertGeometry( nullptr, 0 );
  QVERIFY( gc.isEmpty() );
  QCOMPARE( gc.numGeometries(), 0 );

  gc.insertGeometry( nullptr, -1 );
  QVERIFY( gc.isEmpty() );
  QCOMPARE( gc.numGeometries(), 0 );

  gc.insertGeometry( nullptr, 100 );
  QVERIFY( gc.isEmpty() );
  QCOMPARE( gc.numGeometries(), 0 );

  QgsLineString part1;
  part1.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 5 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 10, 2, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 10, 3, 7 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 0, 4, 8 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 9 ) );

  QgsLineString part2;
  part2.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 2 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 9, 2, 3 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 9, 3, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 1, 4, 4 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 7 ) );

  gc.insertGeometry( part1.clone(), 0 );
  QCOMPARE( gc.numGeometries(), 1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 0 ) ), part1 );

  gc.insertGeometry( part2.clone(), 0 );
  QCOMPARE( gc.numGeometries(), 2 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 0 ) ), part2 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 1 ) ), part1 );

  gc.removeGeometry( 0 );
  gc.insertGeometry( part2.clone(), 1 );
  QCOMPARE( gc.numGeometries(), 2 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 0 ) ), part1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 1 ) ), part2 );

  gc.removeGeometry( 1 );
  gc.insertGeometry( part2.clone(), 2 );
  QCOMPARE( gc.numGeometries(), 2 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 0 ) ), part1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 1 ) ), part2 );
}

void TestQgsGeometryCollection::removeGeometry()
{
  QgsGeometryCollection gc;
  // no crash!
  gc.removeGeometry( -1 );
  gc.removeGeometry( 0 );
  gc.removeGeometry( 100 );

  QgsLineString part1;
  part1.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 5 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 10, 2, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 10, 3, 7 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 0, 4, 8 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 9 ) );
  gc.addGeometry( part1.clone() );

  QgsLineString part2;
  part2.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 2 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 9, 2, 3 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 9, 3, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 1, 4, 4 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 7 ) );
  gc.addGeometry( part2.clone() );

  // no crash
  gc.removeGeometry( -1 );
  gc.removeGeometry( 100 );

  gc.removeGeometry( 0 );
  QCOMPARE( gc.numGeometries(), 1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 0 ) ), part2 );

  gc.addGeometry( part1.clone() );

  gc.removeGeometry( 1 );
  QCOMPARE( gc.numGeometries(), 1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 0 ) ), part2 );

  gc.removeGeometry( 0 );
  QCOMPARE( gc.numGeometries(), 0 );
}

void TestQgsGeometryCollection::assignment()
{
  QgsGeometryCollection gc1;
  QgsGeometryCollection gc2;
  gc1 = gc2;

  QCOMPARE( gc1.numGeometries(), 0 );


  QgsGeometryCollection gc3;
  QgsLineString part1;
  part1.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 5 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 10, 2, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 10, 3, 7 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 0, 4, 8 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 9 ) );
  gc3.addGeometry( part1.clone() );

  QgsLineString part2;
  part2.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 2 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 9, 2, 3 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 9, 3, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 1, 4, 4 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 7 ) );
  gc3.addGeometry( part2.clone() );

  gc1 = gc3;
  QCOMPARE( gc1.numGeometries(), 2 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc1.geometryN( 0 ) ), part1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc1.geometryN( 1 ) ), part2 );
}

void TestQgsGeometryCollection::copy()
{
  QgsGeometryCollection gc1;

  QgsGeometryCollection gc2( gc1 );

  QVERIFY( gc2.isEmpty() );

  QgsLineString part1;
  part1.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 5 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 10, 2, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 10, 3, 7 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 0, 4, 8 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 9 ) );
  gc1.addGeometry( part1.clone() );

  QgsLineString part2;
  part2.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 2 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 9, 2, 3 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 9, 3, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 1, 4, 4 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 7 ) );
  gc1.addGeometry( part2.clone() );

  QgsGeometryCollection gc3( gc1 );

  QCOMPARE( gc3.numGeometries(), 2 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc3.geometryN( 0 ) ), part1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc3.geometryN( 1 ) ), part2 );
}

void TestQgsGeometryCollection::clone()
{
  QgsGeometryCollection gc;

  QgsLineString part1;
  std::unique_ptr< QgsGeometryCollection >cloned( gc.clone() );

  QVERIFY( cloned->isEmpty() );

  part1.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 5 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 10, 2, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 10, 3, 7 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 0, 4, 8 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 9 ) );
  gc.addGeometry( part1.clone() );

  QgsLineString part2;
  part2.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 2 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 9, 2, 3 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 9, 3, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 1, 4, 4 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 7 ) );
  gc.addGeometry( part2.clone() );

  cloned.reset( gc.clone() );

  QCOMPARE( cloned->numGeometries(), 2 );
  QCOMPARE( *static_cast< const QgsLineString * >( cloned->geometryN( 0 ) ), part1 );
  QCOMPARE( *static_cast< const QgsLineString * >( cloned->geometryN( 1 ) ), part2 );
}

void TestQgsGeometryCollection::clear()
{
  QgsGeometryCollection gc;

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZ, 0, 0, 1 )
                  << QgsPoint( QgsWkbTypes::PointZ, 0, 10, 2 )
                  << QgsPoint( QgsWkbTypes::PointZ, 10, 10, 3 )
                  << QgsPoint( QgsWkbTypes::PointZ, 10, 0, 4 )
                  << QgsPoint( QgsWkbTypes::PointZ, 0, 0, 1 ) );
  gc.addGeometry( part.clone() );
  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZ, 1, 1, 1 )
                  << QgsPoint( QgsWkbTypes::PointZ, 1, 9, 2 )
                  << QgsPoint( QgsWkbTypes::PointZ, 9, 9, 3 )
                  << QgsPoint( QgsWkbTypes::PointZ, 9, 1, 4 )
                  << QgsPoint( QgsWkbTypes::PointZ, 1, 1, 1 ) );
  gc.addGeometry( part.clone() );

  QCOMPARE( gc.numGeometries(), 2 );

  gc.clear();

  QVERIFY( gc.isEmpty() );
  QCOMPARE( gc.numGeometries(), 0 );
  QCOMPARE( gc.nCoordinates(), 0 );
  QCOMPARE( gc.ringCount(), 0 );
  QCOMPARE( gc.partCount(), 0 );
  QVERIFY( !gc.is3D() );
  QVERIFY( !gc.isMeasure() );
  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
}

void TestQgsGeometryCollection::equality()
{
  QgsGeometryCollection gc1;
  QgsGeometryCollection gc2;

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 5 )
                  << QgsPoint( QgsWkbTypes::PointZM, 0, 10, 2, 6 )
                  << QgsPoint( QgsWkbTypes::PointZM, 10, 10, 3, 7 )
                  << QgsPoint( QgsWkbTypes::PointZM, 10, 0, 4, 8 )
                  << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 9 ) );
  gc2.addGeometry( part.clone() );

  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 2 )
                  << QgsPoint( QgsWkbTypes::PointZM, 1, 9, 2, 3 )
                  << QgsPoint( QgsWkbTypes::PointZM, 9, 9, 3, 6 )
                  << QgsPoint( QgsWkbTypes::PointZM, 9, 1, 4, 4 )
                  << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 7 ) );
  gc2.addGeometry( part.clone() );

  QVERIFY( !( gc1 == gc2 ) );
  QVERIFY( gc1 != gc2 );

  QgsPoint point;

  QVERIFY( !( gc1 == point ) );
  QVERIFY( gc1 != point );

  QgsMultiPoint mp;
  QgsMultiLineString ml;

  QVERIFY( mp != ml );

  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZ, 0, 0, 1 )
                  << QgsPoint( QgsWkbTypes::PointZ, 0, 10, 2 )
                  << QgsPoint( QgsWkbTypes::PointZ, 10, 10, 3 )
                  << QgsPoint( QgsWkbTypes::PointZ, 10, 0, 4 )
                  << QgsPoint( QgsWkbTypes::PointZ, 0, 0, 1 ) );
  ml.addGeometry( part.clone() );

  QgsMultiLineString ml2;

  QVERIFY( ml != ml2 );

  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZ, 1, 1, 1 )
                  << QgsPoint( QgsWkbTypes::PointZ, 0, 10, 2 )
                  << QgsPoint( QgsWkbTypes::PointZ, 10, 10, 3 )
                  << QgsPoint( QgsWkbTypes::PointZ, 10, 0, 4 )
                  << QgsPoint( QgsWkbTypes::PointZ, 0, 0, 1 ) );
  ml2.addGeometry( part.clone() );

  QVERIFY( ml != ml2 );

  QgsMultiLineString ml3;
  ml3.addGeometry( part.clone() );

  QVERIFY( ml2 == ml3 );
}

void TestQgsGeometryCollection::swapXy()
{
  QgsGeometryCollection gc;
  QgsLineString part;

  gc.swapXy(); // no crash

  part.setPoints( QgsPointSequence() << QgsPoint( 11, 2, 3, 4, QgsWkbTypes::PointZM )
                  << QgsPoint( 11, 12, 13, 14, QgsWkbTypes::PointZM )
                  << QgsPoint( 111, 12, 23, 24, QgsWkbTypes::PointZM ) );
  gc.addGeometry( part.clone() );

  gc.swapXy();
  QCOMPARE( gc.asWkt(), QStringLiteral( "GeometryCollection (LineStringZM (2 11 3 4, 12 11 13 14, 12 111 23 24))" ) );

  part.setPoints( QgsPointSequence() << QgsPoint( 11, 2, 5, 6, QgsWkbTypes::PointZM )
                  << QgsPoint( 11.01, 1.99, 15, 16, QgsWkbTypes::PointZM )
                  << QgsPoint( 11.02, 2.01, 25, 26, QgsWkbTypes::PointZM ) );
  gc.addGeometry( part.clone() );

  gc.swapXy();
  QCOMPARE( gc.asWkt( 2 ), QStringLiteral( "GeometryCollection (LineStringZM (11 2 3 4, 11 12 13 14, 111 12 23 24),LineStringZM (2 11 5 6, 1.99 11.01 15 16, 2.01 11.02 25 26))" ) );
}

void TestQgsGeometryCollection::dropZValue()
{
  QgsGeometryCollection gc;

  gc.dropZValue();
  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2 ) << QgsPoint( 11, 12 ) << QgsPoint( 1, 12 ) << QgsPoint( 1, 2 ) );
  gc.addGeometry( part.clone() );
  gc.addGeometry( part.clone() );

  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );

  gc.dropZValue(); // not z

  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
  QCOMPARE( gc.geometryN( 0 )->wkbType(), QgsWkbTypes::LineString );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 0 ) )->pointN( 0 ), QgsPoint( 1, 2 ) );
  QCOMPARE( gc.geometryN( 1 )->wkbType(), QgsWkbTypes::LineString );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 1 ) )->pointN( 0 ), QgsPoint( 1, 2 ) );

  // with z
  gc.clear();

  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2, 3 ) << QgsPoint( 11, 12, 13 )
                  << QgsPoint( 1, 12, 23 ) << QgsPoint( 1, 2, 3 ) );
  gc.addGeometry( part.clone() );
  gc.addGeometry( part.clone() );

  gc.dropZValue();

  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
  QCOMPARE( gc.geometryN( 0 )->wkbType(), QgsWkbTypes::LineString );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 0 ) )->pointN( 0 ), QgsPoint( 1, 2 ) );
  QCOMPARE( gc.geometryN( 1 )->wkbType(), QgsWkbTypes::LineString );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 1 ) )->pointN( 0 ), QgsPoint( 1, 2 ) );

  // with zm
  gc.clear();

  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2, 3, 4 )
                  << QgsPoint( 11, 12, 13, 14 )
                  << QgsPoint( 1, 12, 23, 24 )
                  << QgsPoint( 1, 2, 3, 4 ) );
  gc.addGeometry( part.clone() );
  gc.addGeometry( part.clone() );

  gc.dropZValue();

  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
  QCOMPARE( gc.geometryN( 0 )->wkbType(), QgsWkbTypes::LineStringM );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 0 ) )->pointN( 0 ), QgsPoint( QgsWkbTypes::PointM, 1, 2, 0, 4 ) );
  QCOMPARE( gc.geometryN( 1 )->wkbType(), QgsWkbTypes::LineStringM );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 1 ) )->pointN( 0 ), QgsPoint( QgsWkbTypes::PointM, 1, 2, 0, 4 ) );
}

void TestQgsGeometryCollection::dropMValue()
{
  QgsGeometryCollection gc;

  gc.dropMValue();
  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2 ) << QgsPoint( 11, 12 )
                  << QgsPoint( 1, 12 ) << QgsPoint( 1, 2 ) );
  gc.addGeometry( part.clone() );
  gc.addGeometry( part.clone() );

  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );

  gc.dropMValue(); // not zm

  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
  QCOMPARE( gc.geometryN( 0 )->wkbType(), QgsWkbTypes::LineString );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 0 ) )->pointN( 0 ), QgsPoint( 1, 2 ) );
  QCOMPARE( gc.geometryN( 1 )->wkbType(), QgsWkbTypes::LineString );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 1 ) )->pointN( 0 ), QgsPoint( 1, 2 ) );

  // with m
  gc.clear();

  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointM,  1, 2, 0, 3 )
                  << QgsPoint( QgsWkbTypes::PointM, 11, 12, 0, 13 )
                  << QgsPoint( QgsWkbTypes::PointM, 1, 12, 0, 23 )
                  << QgsPoint( QgsWkbTypes::PointM,  1, 2, 0, 3 ) );
  gc.addGeometry( part.clone() );
  gc.addGeometry( part.clone() );

  gc.dropMValue();

  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
  QCOMPARE( gc.geometryN( 0 )->wkbType(), QgsWkbTypes::LineString );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 0 ) )->pointN( 0 ), QgsPoint( 1, 2 ) );
  QCOMPARE( gc.geometryN( 1 )->wkbType(), QgsWkbTypes::LineString );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 1 ) )->pointN( 0 ), QgsPoint( 1, 2 ) );

  // with zm
  gc.clear();

  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2, 3, 4 )
                  << QgsPoint( 11, 12, 13, 14 )
                  << QgsPoint( 1, 12, 23, 24 )
                  << QgsPoint( 1, 2, 3, 4 ) );
  gc.addGeometry( part.clone() );
  gc.addGeometry( part.clone() );

  gc.dropMValue();

  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );

  QCOMPARE( gc.geometryN( 0 )->wkbType(), QgsWkbTypes::LineStringZ );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 0 ) )->pointN( 0 ),
            QgsPoint( QgsWkbTypes::PointZ, 1, 2, 3 ) );

  QCOMPARE( gc.geometryN( 1 )->wkbType(), QgsWkbTypes::LineStringZ );
  QCOMPARE( static_cast< const QgsLineString *>( gc.geometryN( 1 ) )->pointN( 0 ),
            QgsPoint( QgsWkbTypes::PointZ, 1, 2, 3 ) );
}

void TestQgsGeometryCollection::closestSegment()
{
  QgsGeometryCollection gc;

  QgsPoint pt;
  QgsVertexId v;
  int leftOf = 0;

  ( void )gc.closestSegment( QgsPoint( 1, 2 ), pt, v ); // empty collection, just want no crash

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 5, 10 ) << QgsPoint( 7, 12 )
                  << QgsPoint( 5, 15 ) << QgsPoint( 5, 10 ) );
  gc.addGeometry( part.clone() );

  QGSCOMPARENEAR( gc.closestSegment( QgsPoint( 4, 11 ), pt, v, &leftOf ), 1.0, 0.0001 );
  QGSCOMPARENEAR( pt.x(), 5, 0.01 );
  QGSCOMPARENEAR( pt.y(), 11, 0.01 );
  QCOMPARE( v, QgsVertexId( 0, 0, 3 ) );
  QCOMPARE( leftOf, 1 );

  QGSCOMPARENEAR( gc.closestSegment( QgsPoint( 8, 11 ), pt, v, &leftOf ),  2.0, 0.0001 );
  QGSCOMPARENEAR( pt.x(), 7, 0.01 );
  QGSCOMPARENEAR( pt.y(), 12, 0.01 );
  QCOMPARE( v, QgsVertexId( 0, 0, 1 ) );
  QCOMPARE( leftOf, 1 );

  QGSCOMPARENEAR( gc.closestSegment( QgsPoint( 6, 11.5 ), pt, v, &leftOf ), 0.125000, 0.0001 );
  QGSCOMPARENEAR( pt.x(), 6.25, 0.01 );
  QGSCOMPARENEAR( pt.y(), 11.25, 0.01 );
  QCOMPARE( v, QgsVertexId( 0, 0, 1 ) );
  QCOMPARE( leftOf, -1 );

  QGSCOMPARENEAR( gc.closestSegment( QgsPoint( 7, 16 ), pt, v, &leftOf ), 4.923077, 0.0001 );
  QGSCOMPARENEAR( pt.x(), 5.153846, 0.01 );
  QGSCOMPARENEAR( pt.y(), 14.769231, 0.01 );
  QCOMPARE( v, QgsVertexId( 0, 0, 2 ) );
  QCOMPARE( leftOf, 1 );

  QGSCOMPARENEAR( gc.closestSegment( QgsPoint( 5.5, 13.5 ), pt, v, &leftOf ), 0.173077, 0.0001 );
  QGSCOMPARENEAR( pt.x(), 5.846154, 0.01 );
  QGSCOMPARENEAR( pt.y(), 13.730769, 0.01 );
  QCOMPARE( v, QgsVertexId( 0, 0, 2 ) );
  QCOMPARE( leftOf, -1 );

  // point directly on segment
  QCOMPARE( gc.closestSegment( QgsPoint( 5, 15 ), pt, v, &leftOf ), 0.0 );
  QCOMPARE( pt, QgsPoint( 5, 15 ) );
  QCOMPARE( v, QgsVertexId( 0, 0, 2 ) );

  // with interior ring
  part.setPoints( QgsPointSequence() << QgsPoint( 6, 11.5 ) << QgsPoint( 6.5, 12 )
                  << QgsPoint( 6, 13 ) << QgsPoint( 6, 11.5 ) );
  gc.addGeometry( part.clone() );

  QGSCOMPARENEAR( gc.closestSegment( QgsPoint( 4, 11 ), pt, v, &leftOf ), 1.0, 0.0001 );
  QGSCOMPARENEAR( pt.x(), 5, 0.01 );
  QGSCOMPARENEAR( pt.y(), 11, 0.01 );
  QCOMPARE( v, QgsVertexId( 0, 0, 3 ) );
  QCOMPARE( leftOf, 1 );

  QGSCOMPARENEAR( gc.closestSegment( QgsPoint( 8, 11 ), pt, v, &leftOf ),  2.0, 0.0001 );
  QGSCOMPARENEAR( pt.x(), 7, 0.01 );
  QGSCOMPARENEAR( pt.y(), 12, 0.01 );
  QCOMPARE( v, QgsVertexId( 0, 0, 1 ) );
  QCOMPARE( leftOf, 1 );

  QGSCOMPARENEAR( gc.closestSegment( QgsPoint( 6, 11.4 ), pt, v, &leftOf ), 0.01, 0.0001 );
  QGSCOMPARENEAR( pt.x(), 6.0, 0.01 );
  QGSCOMPARENEAR( pt.y(), 11.5, 0.01 );
  QCOMPARE( v, QgsVertexId( 1, 0, 1 ) );
  QCOMPARE( leftOf, 1 );

  QGSCOMPARENEAR( gc.closestSegment( QgsPoint( 7, 16 ), pt, v, &leftOf ), 4.923077, 0.0001 );
  QGSCOMPARENEAR( pt.x(), 5.153846, 0.01 );
  QGSCOMPARENEAR( pt.y(), 14.769231, 0.01 );
  QCOMPARE( v, QgsVertexId( 0, 0, 2 ) );
  QCOMPARE( leftOf, 1 );

  QGSCOMPARENEAR( gc.closestSegment( QgsPoint( 5.5, 13.5 ), pt, v, &leftOf ), 0.173077, 0.0001 );
  QGSCOMPARENEAR( pt.x(), 5.846154, 0.01 );
  QGSCOMPARENEAR( pt.y(), 13.730769, 0.01 );
  QCOMPARE( v, QgsVertexId( 0, 0, 2 ) );
  QCOMPARE( leftOf, -1 );

  // point directly on segment
  QCOMPARE( gc.closestSegment( QgsPoint( 6, 13 ), pt, v, &leftOf ), 0.0 );
  QCOMPARE( pt, QgsPoint( 6, 13 ) );
  QCOMPARE( v, QgsVertexId( 1, 0, 2 ) );
  QCOMPARE( leftOf, 0 );
}

void TestQgsGeometryCollection::hasCurvedSegments()
{
  QgsGeometryCollection gc;

  QVERIFY( !gc.hasCurvedSegments() );

  QgsLineString part1;
  part1.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 5 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 10, 2, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 10, 3, 7 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 0, 4, 8 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 9 ) );
  gc.addGeometry( part1.clone() );

  QVERIFY( !gc.hasCurvedSegments() );

  QgsCircularString part2;
  part2.setPoints( QgsPointSequence() << QgsPoint( 1, 2 )
                   << QgsPoint( 11, 10 ) << QgsPoint( 21, 2 ) );
  gc.addGeometry( part2.clone() );

  QVERIFY( gc.hasCurvedSegments() );

}

void TestQgsGeometryCollection::boundary()
{
  QgsGeometryCollection gc;

  // collections have no boundary defined
  QVERIFY( !gc.boundary() );

  // add a geometry and retest, should still be undefined
  QgsLineString *ls = new QgsLineString();
  ls->setPoints( QgsPointSequence() << QgsPoint( 0, 0 ) << QgsPoint( 1, 0 ) );
  gc.addGeometry( ls );

  QVERIFY( !gc.boundary() );
}

void TestQgsGeometryCollection::segmentize()
{
  QgsGeometryCollection gc;

  QgsCircularString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2 )
                  << QgsPoint( 11, 10 ) << QgsPoint( 21, 2 ) );
  gc.addGeometry( part.clone() );

  std::unique_ptr<QgsGeometryCollection> segmentized( static_cast< QgsGeometryCollection * >( gc.segmentize() ) );
  const QgsLineString *segmentizedLine = static_cast< const QgsLineString * >( segmentized->geometryN( 0 ) );

  QCOMPARE( segmentizedLine->numPoints(), 156 );
  QCOMPARE( segmentizedLine->vertexCount(), 156 );
  QCOMPARE( segmentizedLine->ringCount(), 1 );
  QCOMPARE( segmentizedLine->partCount(), 1 );
  QCOMPARE( segmentizedLine->wkbType(), QgsWkbTypes::LineString );
  QVERIFY( !segmentizedLine->is3D() );
  QVERIFY( !segmentizedLine->isMeasure() );

  QCOMPARE( segmentizedLine->pointN( 0 ),
            part.pointN( 0 ) );
  QCOMPARE( segmentizedLine->pointN( segmentizedLine->numPoints() - 1 ),
            part.pointN( part.numPoints() - 1 ) );
}

void TestQgsGeometryCollection::cast()
{
  QVERIFY( !QgsGeometryCollection().cast( nullptr ) );

  QgsGeometryCollection gc;
  QVERIFY( QgsGeometryCollection::cast( &gc ) );

  gc.fromWkt( QStringLiteral( "GeometryCollectionZ(PolygonZ((0 0 0, 0 1 1, 1 0 2, 0 0 0)))" ) );
  QVERIFY( QgsGeometryCollection::cast( &gc ) );

  gc.fromWkt( QStringLiteral( "GeometryCollectionM(PolygonM((0 0 1, 0 1 2, 1 0 3, 0 0 1)))" ) );
  QVERIFY( QgsGeometryCollection::cast( &gc ) );

  gc.fromWkt( QStringLiteral( "GeometryCollectionZM(PolygonZM((0 0 0 1, 0 1 1 2, 1 0 2 3, 0 0 0 1)))" ) );
  QVERIFY( QgsGeometryCollection::cast( &gc ) );
}

void TestQgsGeometryCollection::insertVertex()
{
  QgsGeometryCollection gc;

  //insert vertex in empty collection
  QVERIFY( !gc.insertVertex( QgsVertexId( 0, 0, 0 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( !gc.insertVertex( QgsVertexId( 0, 0, 1 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( !gc.insertVertex( QgsVertexId( 0, 1, 0 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( !gc.insertVertex( QgsVertexId( 1, 0, 0 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( gc.isEmpty() );

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 0, 0 )
                  << QgsPoint( 0.5, 0 ) << QgsPoint( 1, 0 )
                  << QgsPoint( 2, 1 ) << QgsPoint( 1, 2 )
                  << QgsPoint( 0, 2 ) << QgsPoint( 0, 0 ) );
  gc.addGeometry( part.clone() );

  QVERIFY( gc.insertVertex( QgsVertexId( 0, 0, 1 ), QgsPoint( 0.3, 0 ) ) );
  QCOMPARE( gc.nCoordinates(), 8 );

  auto ls = static_cast< const QgsLineString * >( gc.geometryN( 0 ) );
  QCOMPARE( ls->pointN( 0 ), QgsPoint( 0, 0 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 0.3, 0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 0.5, 0 ) );

  QVERIFY( !gc.insertVertex( QgsVertexId( 0, 0, -1 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( !gc.insertVertex( QgsVertexId( 0, 0, 100 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( !gc.insertVertex( QgsVertexId( 1, 0, 0 ), QgsPoint( 6.0, 7.0 ) ) );

  // first vertex
  QVERIFY( gc.insertVertex( QgsVertexId( 0, 0, 0 ), QgsPoint( 0, 0.1 ) ) );

  QCOMPARE( gc.nCoordinates(), 9 );
  QCOMPARE( ls->pointN( 0 ), QgsPoint( 0, 0.1 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 0, 0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 0.3, 0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 0.5, 0 ) );
  QCOMPARE( ls->pointN( 7 ), QgsPoint( 0, 2 ) );
  QCOMPARE( ls->pointN( 8 ), QgsPoint( 0, 0 ) );

  // last vertex
  QVERIFY( gc.insertVertex( QgsVertexId( 0, 0, 9 ), QgsPoint( 0.1, 0.1 ) ) );

  QCOMPARE( gc.nCoordinates(), 10 );
  QCOMPARE( ls->pointN( 0 ), QgsPoint( 0, 0.1 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 0, 0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 0.3, 0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 0.5, 0 ) );
  QCOMPARE( ls->pointN( 8 ), QgsPoint( 0, 0 ) );
  QCOMPARE( ls->pointN( 9 ), QgsPoint( 0.1, 0.1 ) );

  // with second part
  gc.addGeometry( part.clone() );

  QCOMPARE( gc.nCoordinates(), 17 );

  QVERIFY( gc.insertVertex( QgsVertexId( 1, 0, 1 ), QgsPoint( 0.3, 0 ) ) );

  ls = static_cast< const QgsLineString * >( gc.geometryN( 1 ) );
  QCOMPARE( gc.nCoordinates(), 18 );
  QCOMPARE( ls->pointN( 0 ), QgsPoint( 0, 0 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 0.3, 0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 0.5, 0 ) );

  QVERIFY( !gc.insertVertex( QgsVertexId( 1, 0, -1 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( !gc.insertVertex( QgsVertexId( 1, 0, 100 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( !gc.insertVertex( QgsVertexId( 2, 0, 0 ), QgsPoint( 6.0, 7.0 ) ) );

  // first vertex in second part
  QVERIFY( gc.insertVertex( QgsVertexId( 1, 0, 0 ), QgsPoint( 0, 0.1 ) ) );

  QCOMPARE( gc.nCoordinates(), 19 );
  QCOMPARE( ls->pointN( 0 ), QgsPoint( 0, 0.1 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 0, 0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 0.3, 0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 0.5, 0 ) );
  QCOMPARE( ls->pointN( 7 ), QgsPoint( 0, 2 ) );
  QCOMPARE( ls->pointN( 8 ), QgsPoint( 0, 0 ) );

  // last vertex in second part
  QVERIFY( gc.insertVertex( QgsVertexId( 1, 0, 9 ), QgsPoint( 0.1, 0.1 ) ) );

  QCOMPARE( gc.nCoordinates(), 20 );
  QCOMPARE( ls->pointN( 0 ), QgsPoint( 0, 0.1 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 0, 0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 0.3, 0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 0.5, 0 ) );
  QCOMPARE( ls->pointN( 8 ), QgsPoint( 0, 0 ) );
  QCOMPARE( ls->pointN( 9 ), QgsPoint( 0.1, 0.1 ) );
}

void TestQgsGeometryCollection::moveVertex()
{
  QgsGeometryCollection gc;

  //empty collection
  QVERIFY( !gc.moveVertex( QgsVertexId( 0, 0, 0 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( !gc.moveVertex( QgsVertexId( -1, 0, 0 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( !gc.moveVertex( QgsVertexId( 1, 0, 0 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( gc.isEmpty() );

  //valid collection
  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2 )
                  << QgsPoint( 11, 12 ) << QgsPoint( 21, 22 ) << QgsPoint( 1, 2 ) );
  gc.addGeometry( part.clone() );

  QVERIFY( gc.moveVertex( QgsVertexId( 0, 0, 0 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( gc.moveVertex( QgsVertexId( 0, 0, 1 ), QgsPoint( 16.0, 17.0 ) ) );
  QVERIFY( gc.moveVertex( QgsVertexId( 0, 0, 2 ), QgsPoint( 26.0, 27.0 ) ) );

  auto ls = static_cast< const QgsLineString * >( gc.geometryN( 0 ) );
  QCOMPARE( ls->pointN( 0 ), QgsPoint( 6.0, 7.0 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 16.0, 17.0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 26.0, 27.0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 1, 2 ) );

  //out of range
  QVERIFY( !gc.moveVertex( QgsVertexId( 0, 0, -1 ), QgsPoint( 3.0, 4.0 ) ) );
  QVERIFY( !gc.moveVertex( QgsVertexId( 0, 0, 10 ), QgsPoint( 3.0, 4.0 ) ) );
  QVERIFY( !gc.moveVertex( QgsVertexId( 1, 0, 0 ), QgsPoint( 3.0, 4.0 ) ) );

  QCOMPARE( ls->pointN( 0 ), QgsPoint( 6.0, 7.0 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 16.0, 17.0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 26.0, 27.0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 1.0, 2.0 ) );

  // with second part
  gc.addGeometry( part.clone() );

  QVERIFY( gc.moveVertex( QgsVertexId( 1, 0, 0 ), QgsPoint( 6.0, 7.0 ) ) );
  QVERIFY( gc.moveVertex( QgsVertexId( 1, 0, 1 ), QgsPoint( 16.0, 17.0 ) ) );
  QVERIFY( gc.moveVertex( QgsVertexId( 1, 0, 2 ), QgsPoint( 26.0, 27.0 ) ) );

  ls = static_cast< const QgsLineString * >( gc.geometryN( 1 ) );
  QCOMPARE( ls->pointN( 0 ), QgsPoint( 6.0, 7.0 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 16.0, 17.0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 26.0, 27.0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 1.0, 2.0 ) );

  QVERIFY( !gc.moveVertex( QgsVertexId( 1, 0, -1 ), QgsPoint( 3.0, 4.0 ) ) );
  QVERIFY( !gc.moveVertex( QgsVertexId( 1, 0, 10 ), QgsPoint( 3.0, 4.0 ) ) );
  QVERIFY( !gc.moveVertex( QgsVertexId( 2, 0, 0 ), QgsPoint( 3.0, 4.0 ) ) );
}

void TestQgsGeometryCollection::deleteVertex()
{
  QgsGeometryCollection gc;

  //empty collection
  QVERIFY( !gc.deleteVertex( QgsVertexId( 0, 0, 0 ) ) );
  QVERIFY( !gc.deleteVertex( QgsVertexId( 0, 1, 0 ) ) );
  QVERIFY( !gc.deleteVertex( QgsVertexId( 1, 1, 0 ) ) );
  QVERIFY( !gc.deleteVertex( QgsVertexId( -1, 1, 0 ) ) );

  QVERIFY( gc.isEmpty() );

  //valid collection
  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2 )
                  << QgsPoint( 5, 2 ) << QgsPoint( 6, 2 ) << QgsPoint( 7, 2 )
                  << QgsPoint( 11, 12 ) << QgsPoint( 21, 22 ) << QgsPoint( 1, 2 ) );
  gc.addGeometry( part.clone() );

  //out of range vertices
  QVERIFY( !gc.deleteVertex( QgsVertexId( 0, 0, -1 ) ) );
  QVERIFY( !gc.deleteVertex( QgsVertexId( 0, 0, 100 ) ) );
  QVERIFY( !gc.deleteVertex( QgsVertexId( 1, 0, 1 ) ) );

  //valid vertices
  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 1 ) ) );

  auto ls = static_cast< const QgsLineString * >( gc.geometryN( 0 ) );
  QCOMPARE( ls->pointN( 0 ), QgsPoint( 1.0, 2.0 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 6.0, 2.0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 7.0, 2.0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 11.0, 12.0 ) );
  QCOMPARE( ls->pointN( 5 ), QgsPoint( 1.0, 2.0 ) );

  // delete first vertex
  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 0 ) ) );

  QCOMPARE( ls->pointN( 0 ), QgsPoint( 6.0, 2.0 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 7.0, 2.0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 11.0, 12.0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 21.0, 22.0 ) );
  QCOMPARE( ls->pointN( 4 ), QgsPoint( 1.0, 2.0 ) );

  // delete last vertex
  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 4 ) ) );

  QCOMPARE( ls->pointN( 0 ), QgsPoint( 6.0, 2.0 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 7.0, 2.0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 11.0, 12.0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 21.0, 22.0 ) );

  // delete some more vertices - should remove part
  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 0 ) ) );
  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 0 ) ) );
  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 0 ) ) );

  QVERIFY( !gc.geometryN( 0 ) );

  // with two parts
  gc.addGeometry( part.clone() );
  gc.addGeometry( part.clone() );

  //out of range vertices
  QVERIFY( !gc.deleteVertex( QgsVertexId( 1, 0, -1 ) ) );
  QVERIFY( !gc.deleteVertex( QgsVertexId( 1, 0, 100 ) ) );
  QVERIFY( !gc.deleteVertex( QgsVertexId( 2, 0, 1 ) ) );

  //valid vertices
  ls = static_cast< const QgsLineString * >( gc.geometryN( 1 ) );

  QVERIFY( gc.deleteVertex( QgsVertexId( 1, 0, 1 ) ) );

  QCOMPARE( ls->pointN( 0 ), QgsPoint( 1.0, 2.0 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 6.0, 2.0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 7.0, 2.0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 11.0, 12.0 ) );
  QCOMPARE( ls->pointN( 5 ), QgsPoint( 1.0, 2.0 ) );

  // delete first vertex
  QVERIFY( gc.deleteVertex( QgsVertexId( 1, 0, 0 ) ) );

  QCOMPARE( ls->pointN( 0 ), QgsPoint( 6.0, 2.0 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 7.0, 2.0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 11.0, 12.0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 21.0, 22.0 ) );
  QCOMPARE( ls->pointN( 4 ), QgsPoint( 1.0, 2.0 ) );

  // delete last vertex
  QVERIFY( gc.deleteVertex( QgsVertexId( 1, 0, 4 ) ) );

  QCOMPARE( ls->pointN( 0 ), QgsPoint( 6.0, 2.0 ) );
  QCOMPARE( ls->pointN( 1 ), QgsPoint( 7.0, 2.0 ) );
  QCOMPARE( ls->pointN( 2 ), QgsPoint( 11.0, 12.0 ) );
  QCOMPARE( ls->pointN( 3 ), QgsPoint( 21.0, 22.0 ) );

  // delete some more vertices - should remove part
  QVERIFY( gc.deleteVertex( QgsVertexId( 1, 0, 1 ) ) );
  QVERIFY( gc.deleteVertex( QgsVertexId( 1, 0, 1 ) ) );
  QVERIFY( gc.deleteVertex( QgsVertexId( 1, 0, 1 ) ) );

  QCOMPARE( gc.numGeometries(), 1 );
  QVERIFY( gc.geometryN( 0 ) );

  // test that second geometry is "promoted" when first is removed
  gc.addGeometry( part.clone() );

  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 0 ) ) );
  QCOMPARE( gc.numGeometries(), 2 );

  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 0 ) ) );
  QCOMPARE( gc.numGeometries(), 2 );

  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 0 ) ) );
  QCOMPARE( gc.numGeometries(), 2 );

  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 0 ) ) );
  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 0 ) ) );
  QVERIFY( gc.deleteVertex( QgsVertexId( 0, 0, 0 ) ) );

  QCOMPARE( gc.numGeometries(), 1 );
  QVERIFY( gc.geometryN( 0 ) );
}

void TestQgsGeometryCollection::vertexNumberFromVertexId()
{
  QgsGeometryCollection gc;

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2 )
                  << QgsPoint( 11, 12 ) << QgsPoint( 111, 112 ) );

  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( -1, 0, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 1, 0, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, -1, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 1, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, -1 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 1 ) ), -1 );

  gc.addGeometry( part.clone() );

  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( -1, 0, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 1, 0, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, -1, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 1, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, -1 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 0 ) ), 0 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 1 ) ), 1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 2 ) ), 2 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 3 ) ), -1 );

  gc.addGeometry( part.clone() );

  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 0 ) ), 0 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 1 ) ), 1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 2 ) ), 2 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 3 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 1, 0, 0 ) ), 3 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 1, 0, 1 ) ), 4 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 1, 0, 2 ) ), 5 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 1, 0, 3 ) ), -1 );

  QgsPolygon polyPart;
  part.close();
  polyPart.setExteriorRing( part.clone() );
  gc.addGeometry( polyPart.clone() );

  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 0 ) ), 0 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 1 ) ), 1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 2 ) ), 2 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 0, 0, 3 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 1, 0, 0 ) ), 3 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 1, 0, 1 ) ), 4 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 1, 0, 2 ) ), 5 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 1, 0, 3 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, -1, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 1, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 0, -1 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 0, 0 ) ), 6 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 0, 1 ) ), 7 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 0, 2 ) ), 8 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 0, 3 ) ), 9 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 0, 4 ) ), -1 );

  polyPart.addInteriorRing( part.clone() );
  gc.addGeometry( polyPart.clone() );

  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 0, 0 ) ), 6 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 0, 1 ) ), 7 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 0, 2 ) ), 8 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 0, 3 ) ), 9 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 2, 0, 4 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, -1, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 2, 0 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 0, 0 ) ), 10 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 0, 1 ) ), 11 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 0, 2 ) ), 12 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 0, 3 ) ), 13 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 0, 4 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 1, 0 ) ), 14 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 1, 1 ) ), 15 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 1, 2 ) ), 16 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 1, 3 ) ), 17 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 1, 4 ) ), -1 );
  QCOMPARE( gc.vertexNumberFromVertexId( QgsVertexId( 3, 2, 0 ) ), -1 );
}

void TestQgsGeometryCollection::nextVertex()
{
  QgsGeometryCollection gc;

  QgsPoint pt;
  QgsVertexId v;
  QVERIFY( !gc.nextVertex( v, pt ) );

  v = QgsVertexId( 0, 0, -2 );
  QVERIFY( !gc.nextVertex( v, pt ) );

  v = QgsVertexId( 0, 0, 10 );
  QVERIFY( !gc.nextVertex( v, pt ) );

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2 ) << QgsPoint( 11, 12 )
                  << QgsPoint( 1, 12 ) << QgsPoint( 1, 2 ) );
  gc.addGeometry( part.clone() );

  v = QgsVertexId( 0, 0, 4 ); //out of range
  QVERIFY( !gc.nextVertex( v, pt ) );

  v = QgsVertexId( 0, 0, -5 );
  QVERIFY( gc.nextVertex( v, pt ) );

  v = QgsVertexId( 0, 0, -1 );
  QVERIFY( gc.nextVertex( v, pt ) );
  QCOMPARE( v, QgsVertexId( 0, 0, 0 ) );
  QCOMPARE( pt, QgsPoint( 1, 2 ) );

  QVERIFY( gc.nextVertex( v, pt ) );
  QCOMPARE( v, QgsVertexId( 0, 0, 1 ) );
  QCOMPARE( pt, QgsPoint( 11, 12 ) );

  QVERIFY( gc.nextVertex( v, pt ) );
  QCOMPARE( v, QgsVertexId( 0, 0, 2 ) );
  QCOMPARE( pt, QgsPoint( 1, 12 ) );

  QVERIFY( gc.nextVertex( v, pt ) );
  QCOMPARE( v, QgsVertexId( 0, 0, 3 ) );
  QCOMPARE( pt, QgsPoint( 1, 2 ) );

  v = QgsVertexId( 1, 0, 0 );
  QVERIFY( !gc.nextVertex( v, pt ) );

  // add another part
  part.setPoints( QgsPointSequence() << QgsPoint( 11, 12 ) << QgsPoint( 21, 22 )
                  << QgsPoint( 11, 22 ) << QgsPoint( 11, 12 ) );
  gc.addGeometry( part.clone() );

  v = QgsVertexId( 1, 0, 4 ); //out of range
  QVERIFY( !gc.nextVertex( v, pt ) );

  v = QgsVertexId( 1, 0, -5 );
  QVERIFY( gc.nextVertex( v, pt ) );

  v = QgsVertexId( 1, 0, -1 );
  QVERIFY( gc.nextVertex( v, pt ) );
  QCOMPARE( v, QgsVertexId( 1, 0, 0 ) );
  QCOMPARE( pt, QgsPoint( 11, 12 ) );

  QVERIFY( gc.nextVertex( v, pt ) );
  QCOMPARE( v, QgsVertexId( 1, 0, 1 ) );
  QCOMPARE( pt, QgsPoint( 21, 22 ) );

  QVERIFY( gc.nextVertex( v, pt ) );
  QCOMPARE( v, QgsVertexId( 1, 0, 2 ) );
  QCOMPARE( pt, QgsPoint( 11, 22 ) );

  QVERIFY( gc.nextVertex( v, pt ) );
  QCOMPARE( v, QgsVertexId( 1, 0, 3 ) );
  QCOMPARE( pt, QgsPoint( 11, 12 ) );

  v = QgsVertexId( 2, 0, 0 );
  QVERIFY( !gc.nextVertex( v, pt ) );

  v = QgsVertexId( 1, 1, 0 );
  QVERIFY( gc.nextVertex( v, pt ) );
  QCOMPARE( v, QgsVertexId( 1, 1, 1 ) ); //test that part number is maintained
  QCOMPARE( pt, QgsPoint( 21, 22 ) );
}

void TestQgsGeometryCollection::adjacentVertices()
{
  QgsGeometryCollection gc;
  QgsLineString part;

  QgsVertexId prev( 1, 2, 3 ); // start with something
  QgsVertexId next( 4, 5, 6 );

  gc.adjacentVertices( QgsVertexId( 0, 0, -1 ), prev, next );
  QCOMPARE( prev, QgsVertexId() );
  QCOMPARE( next, QgsVertexId() );

  gc.adjacentVertices( QgsVertexId( -1, 0, -1 ), prev, next );
  QCOMPARE( prev, QgsVertexId() );
  QCOMPARE( next, QgsVertexId() );

  gc.adjacentVertices( QgsVertexId( 10, 0, -1 ), prev, next );
  QCOMPARE( prev, QgsVertexId() );
  QCOMPARE( next, QgsVertexId() );

  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2 )
                  << QgsPoint( 11, 12 ) << QgsPoint( 111, 112 ) );
  gc.addGeometry( part.clone() );
  gc.addGeometry( part.clone() );

  gc.adjacentVertices( QgsVertexId( -1, 0, -1 ), prev, next );
  QCOMPARE( prev, QgsVertexId() );
  QCOMPARE( next, QgsVertexId() );

  gc.adjacentVertices( QgsVertexId( 10, 0, -1 ), prev, next );
  QCOMPARE( prev, QgsVertexId() );
  QCOMPARE( next, QgsVertexId() );

  gc.adjacentVertices( QgsVertexId( 0, 0, 0 ), prev, next );
  QCOMPARE( prev, QgsVertexId() );
  QCOMPARE( next, QgsVertexId( 0, 0, 1 ) );

  gc.adjacentVertices( QgsVertexId( 0, 0, 1 ), prev, next );
  QCOMPARE( prev, QgsVertexId( 0, 0, 0 ) );
  QCOMPARE( next, QgsVertexId( 0, 0, 2 ) );

  gc.adjacentVertices( QgsVertexId( 1, 0, 1 ), prev, next );
  QCOMPARE( prev, QgsVertexId( 1, 0, 0 ) );
  QCOMPARE( next, QgsVertexId( 1, 0, 2 ) );
}

void TestQgsGeometryCollection::vertexAngle()
{
  QgsGeometryCollection gc;

  ( void )gc.vertexAngle( QgsVertexId() ); //just want no crash
  ( void )gc.vertexAngle( QgsVertexId( 0, 0, 0 ) ); //just want no crash
  ( void )gc.vertexAngle( QgsVertexId( 0, 1, 0 ) ); //just want no crash
  ( void )gc.vertexAngle( QgsVertexId( 1, 0, 0 ) ); //just want no crash
  ( void )gc.vertexAngle( QgsVertexId( -1, 0, 0 ) ); //just want no crash

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 0, 0 )
                  << QgsPoint( 0.5, 0 ) << QgsPoint( 1, 0 )
                  << QgsPoint( 2, 1 ) << QgsPoint( 1, 2 )
                  << QgsPoint( 0, 2 ) << QgsPoint( 0, 0 ) );
  gc.addGeometry( part.clone() );

  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 0, 0, 0 ) ), 2.35619, 0.00001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 0, 0, 1 ) ), 1.5708, 0.0001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 0, 0, 2 ) ), 1.17809, 0.00001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 0, 0, 3 ) ), 0.0, 0.00001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 0, 0, 4 ) ), 5.10509, 0.00001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 0, 0, 5 ) ), 3.92699, 0.00001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 0, 0, 6 ) ), 2.35619, 0.00001 );

  gc.addGeometry( part.clone() );

  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 1, 0, 0 ) ), 2.35619, 0.00001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 1, 0, 1 ) ), 1.5708, 0.0001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 1, 0, 2 ) ), 1.17809, 0.00001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 1, 0, 3 ) ), 0.0, 0.00001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 1, 0, 4 ) ), 5.10509, 0.00001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 1, 0, 5 ) ), 3.92699, 0.00001 );
  QGSCOMPARENEAR( gc.vertexAngle( QgsVertexId( 1, 0, 6 ) ), 2.35619, 0.00001 );
}

void TestQgsGeometryCollection::removeDuplicateNodes()
{
  QgsGeometryCollection gc;
  QgsLineString part;

  QVERIFY( !gc.removeDuplicateNodes() );

  part.setPoints( QgsPointSequence() << QgsPoint( 11, 2 )
                  << QgsPoint( 11, 12 ) << QgsPoint( 111, 12 ) );
  gc.addGeometry( part.clone() );

  QVERIFY( !gc.removeDuplicateNodes( 0.02 ) );
  QCOMPARE( gc.asWkt(), QStringLiteral( "GeometryCollection (LineString (11 2, 11 12, 111 12))" ) );

  part.setPoints( QgsPointSequence() << QgsPoint( 11, 2 )
                  << QgsPoint( 11.01, 1.99 ) << QgsPoint( 11.02, 2.01 )
                  << QgsPoint( 11, 12 ) << QgsPoint( 111, 12 )
                  << QgsPoint( 111.01, 11.99 ) );
  gc.addGeometry( part.clone() );

  QVERIFY( gc.removeDuplicateNodes( 0.02 ) );
  QVERIFY( !gc.removeDuplicateNodes( 0.02 ) );
  QCOMPARE( gc.asWkt( 2 ), QStringLiteral( "GeometryCollection (LineString (11 2, 11 12, 111 12),LineString (11 2, 11 12, 111 12))" ) );
}

void TestQgsGeometryCollection::filterVertices()
{
  QgsGeometryCollection gc;

  auto filter = []( const QgsPoint & point )-> bool
  {
    return point.x() > 5;
  };

  gc.filterVertices( filter ); // no crash

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2, 3, 4, QgsWkbTypes::PointZM )
                  << QgsPoint( 11, 12, 13, 14, QgsWkbTypes::PointZM )
                  << QgsPoint( 111, 12, 23, 24, QgsWkbTypes::PointZM ) );
  gc.addGeometry( part.clone() );

  gc.filterVertices( filter );
  QCOMPARE( gc.asWkt(), QStringLiteral( "GeometryCollection (LineStringZM (11 12 13 14, 111 12 23 24))" ) );

  part.setPoints( QgsPointSequence() << QgsPoint( 11, 2, 5, 6, QgsWkbTypes::PointZM )
                  << QgsPoint( 1.01, 1.99, 15, 16, QgsWkbTypes::PointZM )
                  << QgsPoint( 11.02, 2.01, 25, 26, QgsWkbTypes::PointZM ) );
  gc.addGeometry( part.clone() );

  gc.filterVertices( filter );
  QCOMPARE( gc.asWkt( 2 ), QStringLiteral( "GeometryCollection (LineStringZM (11 12 13 14, 111 12 23 24),LineStringZM (11 2 5 6, 11.02 2.01 25 26))" ) );
}

void TestQgsGeometryCollection::transformVertices()
{
  QgsGeometryCollection gc;

  auto transform = []( const QgsPoint & point )-> QgsPoint
  {
    return QgsPoint( point.x() + 2, point.y() + 3, point.z() + 4, point.m() + 5 );
  };

  gc.transformVertices( transform ); // no crash

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2, 3, 4, QgsWkbTypes::PointZM )
                  << QgsPoint( 11, 12, 13, 14, QgsWkbTypes::PointZM )
                  << QgsPoint( 111, 12, 23, 24, QgsWkbTypes::PointZM ) );
  gc.addGeometry( part.clone() );

  gc.transformVertices( transform );
  QCOMPARE( gc.asWkt(), QStringLiteral( "GeometryCollection (LineStringZM (3 5 7 9, 13 15 17 19, 113 15 27 29))" ) );

  part.setPoints( QgsPointSequence() << QgsPoint( 11, 2, 5, 6, QgsWkbTypes::PointZM )
                  << QgsPoint( 1.01, 1.99, 15, 16, QgsWkbTypes::PointZM )
                  << QgsPoint( 11.02, 2.01, 25, 26, QgsWkbTypes::PointZM ) );
  gc.addGeometry( part.clone() );

  gc.transformVertices( transform );
  QCOMPARE( gc.asWkt( 2 ), QStringLiteral( "GeometryCollection (LineStringZM (5 8 11 14, 15 18 21 24, 115 18 31 34),LineStringZM (13 5 9 11, 3.01 4.99 19 21, 13.02 5.01 29 31))" ) );
}

void TestQgsGeometryCollection::transformWithClass()
{
  QgsGeometryCollection gc;
  TestTransformer transformer;

  gc.transform( &transformer ); // no crash

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 1, 2, 3, 4, QgsWkbTypes::PointZM )
                  << QgsPoint( 11, 12, 13, 14, QgsWkbTypes::PointZM )
                  << QgsPoint( 111, 12, 23, 24, QgsWkbTypes::PointZM ) );
  gc.addGeometry( part.clone() );

  QVERIFY( gc.transform( &transformer ) );
  QCOMPARE( gc.asWkt(), QStringLiteral( "GeometryCollection (LineStringZM (3 16 8 3, 33 26 18 13, 333 26 28 23))" ) );

  part.setPoints( QgsPointSequence() << QgsPoint( 11, 2, 5, 6, QgsWkbTypes::PointZM )
                  << QgsPoint( 1.01, 1.99, 15, 16, QgsWkbTypes::PointZM )
                  << QgsPoint( 11.02, 2.01, 25, 26, QgsWkbTypes::PointZM ) );
  gc.addGeometry( part.clone() );

  QVERIFY( gc.transform( &transformer ) );
  QCOMPARE( gc.asWkt( 2 ), QStringLiteral( "GeometryCollection (LineStringZM (9 30 13 2, 99 40 23 12, 999 40 33 22),LineStringZM (33 16 10 5, 3.03 15.99 20 15, 33.06 16.01 30 25))" ) );

  TestFailTransformer failTransformer;
  QVERIFY( !gc.transform( &failTransformer ) );
}

void TestQgsGeometryCollection::transform()
{
  //CRS transform
  QgsCoordinateReferenceSystem sourceSrs( QStringLiteral( "EPSG:3994" ) );
  QgsCoordinateReferenceSystem destSrs( QStringLiteral( "EPSG:4202" ) ); // want a transform with ellipsoid change
  QgsCoordinateTransform tr( sourceSrs, destSrs, QgsProject::instance() );

  // 2d CRS transform
  QgsGeometryCollection gc;
  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( 6374985, -3626584 )
                  << QgsPoint( 6274985, -3526584 )
                  << QgsPoint( 6474985, -3526584 )
                  << QgsPoint( 6374985, -3626584 ) );
  gc.addGeometry( part.clone() );
  gc.addGeometry( part.clone() );

  gc.transform( tr, Qgis::TransformDirection::Forward );

  auto *extR = static_cast< const QgsLineString * >( gc.geometryN( 0 ) );
  QGSCOMPARENEAR( extR->pointN( 0 ).x(), 175.771, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 0 ).y(), -39.724, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).x(),  174.581448, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).y(), -38.7999, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).x(),  176.958633, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).y(), -38.7999, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).x(),  175.771, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).y(), -39.724, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().xMinimum(), 174.581448, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().yMinimum(), -39.724, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().xMaximum(), 176.959, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().yMaximum(), -38.7999, 0.001 );

  auto *intR = static_cast< const QgsLineString * >( gc.geometryN( 1 ) );
  QGSCOMPARENEAR( intR->pointN( 0 ).x(), 175.771, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 0 ).y(), -39.724, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).x(),  174.581448, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).y(), -38.7999, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).x(),  176.958633, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).y(), -38.7999, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).x(),  175.771, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).y(), -39.724, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().xMinimum(), 174.581448, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().yMinimum(), -39.724, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().xMaximum(), 176.959, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().yMaximum(), -38.7999, 0.001 );

  //3d CRS transform
  gc.clear();

  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 6374985, -3626584, 1, 2 )
                  << QgsPoint( QgsWkbTypes::PointZM, 6274985, -3526584, 3, 4 )
                  << QgsPoint( QgsWkbTypes::PointZM, 6474985, -3526584, 5, 6 )
                  << QgsPoint( QgsWkbTypes::PointZM, 6374985, -3626584, 1, 2 ) );
  gc.addGeometry( part.clone() );
  gc.addGeometry( part.clone() );

  gc.transform( tr, Qgis::TransformDirection::Forward );

  extR = static_cast< const QgsLineString * >( gc.geometryN( 0 ) );
  QGSCOMPARENEAR( extR->pointN( 0 ).x(), 175.771, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 0 ).y(), -39.724, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 0 ).z(), 1.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 0 ).m(), 2.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).x(),  174.581448, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).y(), -38.7999, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).z(), 3.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).m(), 4.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).x(),  176.958633, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).y(), -38.7999, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).z(), 5.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).m(), 6.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).x(),  175.771, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).y(), -39.724, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).z(), 1.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).m(), 2.0, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().xMinimum(), 174.581448, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().yMinimum(), -39.724, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().xMaximum(), 176.959, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().yMaximum(), -38.7999, 0.001 );

  intR = static_cast< const QgsLineString * >( gc.geometryN( 1 ) );
  QGSCOMPARENEAR( intR->pointN( 0 ).x(), 175.771, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 0 ).y(), -39.724, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 0 ).z(), 1.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 0 ).m(), 2.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).x(),  174.581448, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).y(), -38.7999, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).z(), 3.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).m(), 4.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).x(),  176.958633, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).y(), -38.7999, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).z(), 5.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).m(), 6.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).x(),  175.771, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).y(), -39.724, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).z(), 1.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).m(), 2.0, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().xMinimum(), 174.581448, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().yMinimum(), -39.724, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().xMaximum(), 176.959, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().yMaximum(), -38.7999, 0.001 );

  //reverse transform
  gc.transform( tr, Qgis::TransformDirection::Reverse );

  extR = static_cast< const QgsLineString * >( gc.geometryN( 0 ) );
  QGSCOMPARENEAR( extR->pointN( 0 ).x(), 6374984, 100 );
  QGSCOMPARENEAR( extR->pointN( 0 ).y(), -3626584, 100 );
  QGSCOMPARENEAR( extR->pointN( 0 ).z(), 1.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 0 ).m(), 2.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).x(), 6274984, 100 );
  QGSCOMPARENEAR( extR->pointN( 1 ).y(), -3526584, 100 );
  QGSCOMPARENEAR( extR->pointN( 1 ).z(), 3.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).m(), 4.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).x(),  6474984, 100 );
  QGSCOMPARENEAR( extR->pointN( 2 ).y(), -3526584, 100 );
  QGSCOMPARENEAR( extR->pointN( 2 ).z(), 5.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).m(), 6.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).x(),  6374984, 100 );
  QGSCOMPARENEAR( extR->pointN( 3 ).y(), -3626584, 100 );
  QGSCOMPARENEAR( extR->pointN( 3 ).z(), 1.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).m(), 2.0, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().xMinimum(), 6274984, 100 );
  QGSCOMPARENEAR( extR->boundingBox().yMinimum(), -3626584, 100 );
  QGSCOMPARENEAR( extR->boundingBox().xMaximum(), 6474984, 100 );
  QGSCOMPARENEAR( extR->boundingBox().yMaximum(), -3526584, 100 );

  intR = static_cast< const QgsLineString * >( gc.geometryN( 1 ) );
  QGSCOMPARENEAR( intR->pointN( 0 ).x(), 6374984, 100 );
  QGSCOMPARENEAR( intR->pointN( 0 ).y(), -3626584, 100 );
  QGSCOMPARENEAR( intR->pointN( 0 ).z(), 1.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 0 ).m(), 2.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).x(), 6274984, 100 );
  QGSCOMPARENEAR( intR->pointN( 1 ).y(), -3526584, 100 );
  QGSCOMPARENEAR( intR->pointN( 1 ).z(), 3.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).m(), 4.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).x(),  6474984, 100 );
  QGSCOMPARENEAR( intR->pointN( 2 ).y(), -3526584, 100 );
  QGSCOMPARENEAR( intR->pointN( 2 ).z(), 5.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).m(), 6.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).x(),  6374984, 100 );
  QGSCOMPARENEAR( intR->pointN( 3 ).y(), -3626584, 100 );
  QGSCOMPARENEAR( intR->pointN( 3 ).z(), 1.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).m(), 2.0, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().xMinimum(), 6274984, 100 );
  QGSCOMPARENEAR( intR->boundingBox().yMinimum(), -3626584, 100 );
  QGSCOMPARENEAR( intR->boundingBox().xMaximum(), 6474984, 100 );
  QGSCOMPARENEAR( intR->boundingBox().yMaximum(), -3526584, 100 );

#if PROJ_VERSION_MAJOR<6 // note - z value transform doesn't currently work with proj 6+, because we don't yet support compound CRS definitions
  //z value transform
  gc.transform( tr, Qgis::TransformDirection::Forward, true );

  extR = static_cast< const QgsLineString * >( gc.geometryN( 0 ) );
  QGSCOMPARENEAR( extR->pointN( 0 ).z(), -19.249066, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).z(), -19.148357, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).z(), -19.092128, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).z(), -19.249066, 0.001 );

  intR = static_cast< const QgsLineString * >( gc.geometryN( 1 ) );
  QGSCOMPARENEAR( intR->pointN( 0 ).z(), -19.249066, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).z(), -19.148357, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).z(), -19.092128, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).z(), -19.249066, 0.001 );

  gc.transform( tr, Qgis::TransformDirection::Reverse, true );

  extR = static_cast< const QgsLineString * >( gc.geometryN( 0 ) );
  QGSCOMPARENEAR( extR->pointN( 0 ).z(), 1, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).z(), 3, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).z(), 5, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).z(), 1, 0.001 );

  intR = static_cast< const QgsLineString * >( gc.geometryN( 1 ) );
  QGSCOMPARENEAR( intR->pointN( 0 ).z(), 1, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).z(), 3, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).z(), 5, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).z(), 1, 0.001 );
#endif
}

void TestQgsGeometryCollection::qTransform()
{
  QgsGeometryCollection gc;

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 1, 2, 3, 4 )
                  << QgsPoint( QgsWkbTypes::PointZM, 11, 12, 13, 14 )
                  << QgsPoint( QgsWkbTypes::PointZM, 1, 12, 23, 24 )
                  << QgsPoint( QgsWkbTypes::PointZM, 1, 2, 3, 4 ) );
  gc.addGeometry( part.clone() );
  gc.addGeometry( part.clone() );

  QTransform qtr = QTransform::fromScale( 2, 3 );
  gc.transform( qtr, 3, 2, 6, 3 );

  auto *extR = static_cast< const QgsLineString * >( gc.geometryN( 0 ) );
  QGSCOMPARENEAR( extR->pointN( 0 ).x(), 2, 100 );
  QGSCOMPARENEAR( extR->pointN( 0 ).y(), 6, 100 );
  QGSCOMPARENEAR( extR->pointN( 0 ).z(), 9.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 0 ).m(), 18.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).x(), 22, 100 );
  QGSCOMPARENEAR( extR->pointN( 1 ).y(), 36, 100 );
  QGSCOMPARENEAR( extR->pointN( 1 ).z(), 29.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 1 ).m(), 48.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).x(),  2, 100 );
  QGSCOMPARENEAR( extR->pointN( 2 ).y(), 36, 100 );
  QGSCOMPARENEAR( extR->pointN( 2 ).z(), 49.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 2 ).m(), 78.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).x(), 2, 100 );
  QGSCOMPARENEAR( extR->pointN( 3 ).y(), 6, 100 );
  QGSCOMPARENEAR( extR->pointN( 3 ).z(), 9.0, 0.001 );
  QGSCOMPARENEAR( extR->pointN( 3 ).m(), 18.0, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().xMinimum(), 2, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().yMinimum(), 6, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().xMaximum(), 22, 0.001 );
  QGSCOMPARENEAR( extR->boundingBox().yMaximum(), 36, 0.001 );

  auto *intR = static_cast< const QgsLineString * >( gc.geometryN( 1 ) );
  QGSCOMPARENEAR( intR->pointN( 0 ).x(), 2, 100 );
  QGSCOMPARENEAR( intR->pointN( 0 ).y(), 6, 100 );
  QGSCOMPARENEAR( intR->pointN( 0 ).z(), 9.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 0 ).m(), 18.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).x(), 22, 100 );
  QGSCOMPARENEAR( intR->pointN( 1 ).y(), 36, 100 );
  QGSCOMPARENEAR( intR->pointN( 1 ).z(), 29.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 1 ).m(), 48.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).x(),  2, 100 );
  QGSCOMPARENEAR( intR->pointN( 2 ).y(), 36, 100 );
  QGSCOMPARENEAR( intR->pointN( 2 ).z(), 49.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 2 ).m(), 78.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).x(), 2, 100 );
  QGSCOMPARENEAR( intR->pointN( 3 ).y(), 6, 100 );
  QGSCOMPARENEAR( intR->pointN( 3 ).z(), 9.0, 0.001 );
  QGSCOMPARENEAR( intR->pointN( 3 ).m(), 18.0, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().xMinimum(), 2, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().yMinimum(), 6, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().xMaximum(), 22, 0.001 );
  QGSCOMPARENEAR( intR->boundingBox().yMaximum(), 36, 0.001 );
}

void TestQgsGeometryCollection::toCurveType()
{
  QgsGeometryCollection gc;

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 5 )
                  << QgsPoint( QgsWkbTypes::PointZM, 0, 10, 2, 6 )
                  << QgsPoint( QgsWkbTypes::PointZM, 10, 10, 3, 7 )
                  << QgsPoint( QgsWkbTypes::PointZM, 10, 0, 4, 8 )
                  << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 9 ) );
  gc.addGeometry( part.clone() );

  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 2 )
                  << QgsPoint( QgsWkbTypes::PointZM, 1, 9, 2, 3 )
                  << QgsPoint( QgsWkbTypes::PointZM, 9, 9, 3, 6 )
                  << QgsPoint( QgsWkbTypes::PointZM, 9, 1, 4, 4 )
                  << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 7 ) );
  gc.addGeometry( part.clone() );

  std::unique_ptr< QgsGeometryCollection > curveType( gc.toCurveType() );

  QCOMPARE( curveType->wkbType(), QgsWkbTypes::GeometryCollection );
  QCOMPARE( curveType->numGeometries(), 2 );

  auto *curve = static_cast< const QgsCompoundCurve * >( curveType->geometryN( 0 ) );
  QCOMPARE( curve->numPoints(), 5 );
  QCOMPARE( curve->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 5 ) );
  QCOMPARE( curve->vertexAt( QgsVertexId( 0, 0, 1 ) ), QgsPoint( QgsWkbTypes::PointZM, 0, 10, 2, 6 ) );
  QCOMPARE( curve->vertexAt( QgsVertexId( 0, 0, 2 ) ), QgsPoint( QgsWkbTypes::PointZM, 10, 10, 3, 7 ) );
  QCOMPARE( curve->vertexAt( QgsVertexId( 0, 0, 3 ) ), QgsPoint( QgsWkbTypes::PointZM, 10, 0, 4, 8 ) );
  QCOMPARE( curve->vertexAt( QgsVertexId( 0, 0, 4 ) ), QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 9 ) );

  curve = static_cast< const QgsCompoundCurve * >( curveType->geometryN( 1 ) );
  QCOMPARE( curve->numPoints(), 5 );
  QCOMPARE( curve->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 2 ) );
  QCOMPARE( curve->vertexAt( QgsVertexId( 0, 0, 1 ) ), QgsPoint( QgsWkbTypes::PointZM, 1, 9, 2, 3 ) );
  QCOMPARE( curve->vertexAt( QgsVertexId( 0, 0, 2 ) ), QgsPoint( QgsWkbTypes::PointZM, 9, 9, 3, 6 ) );
  QCOMPARE( curve->vertexAt( QgsVertexId( 0, 0, 3 ) ), QgsPoint( QgsWkbTypes::PointZM, 9, 1, 4, 4 ) );
  QCOMPARE( curve->vertexAt( QgsVertexId( 0, 0, 4 ) ), QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 7 ) );
}

void TestQgsGeometryCollection::toFromWKB()
{
  QgsGeometryCollection gc1;

  QgsLineString part1;
  part1.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::Point, 0, 0 )
                   << QgsPoint( QgsWkbTypes::Point, 0, 10 )
                   << QgsPoint( QgsWkbTypes::Point, 10, 10 )
                   << QgsPoint( QgsWkbTypes::Point, 10, 0 )
                   << QgsPoint( QgsWkbTypes::Point, 0, 0 ) );
  gc1.addGeometry( part1.clone() );

  QgsLineString part2;
  part2.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::Point, 1, 1 )
                   << QgsPoint( QgsWkbTypes::Point, 1, 9 )
                   << QgsPoint( QgsWkbTypes::Point, 9, 9 )
                   << QgsPoint( QgsWkbTypes::Point, 9, 1 )
                   << QgsPoint( QgsWkbTypes::Point, 1, 1 ) );
  gc1.addGeometry( part2.clone() );

  QByteArray wkb = gc1.asWkb();
  QCOMPARE( wkb.size(), gc1.wkbSize() );

  QgsGeometryCollection gc2;
  QgsConstWkbPtr wkbPtr1( wkb );
  gc2.fromWkb( wkbPtr1 );

  QCOMPARE( gc2.numGeometries(), 2 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc2.geometryN( 0 ) ), part1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc2.geometryN( 1 ) ), part2 );

  //parts with Z
  gc1.clear();
  gc2.clear();

  part1.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZ, 0, 0, 1 )
                   << QgsPoint( QgsWkbTypes::PointZ, 0, 10, 2 )
                   << QgsPoint( QgsWkbTypes::PointZ, 10, 10, 3 )
                   << QgsPoint( QgsWkbTypes::PointZ, 10, 0, 4 )
                   << QgsPoint( QgsWkbTypes::PointZ, 0, 0, 1 ) );
  gc1.addGeometry( part1.clone() );

  part2.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZ, 1, 1, 1 )
                   << QgsPoint( QgsWkbTypes::PointZ, 1, 9, 2 )
                   << QgsPoint( QgsWkbTypes::PointZ, 9, 9, 3 )
                   << QgsPoint( QgsWkbTypes::PointZ, 9, 1, 4 )
                   << QgsPoint( QgsWkbTypes::PointZ, 1, 1, 1 ) );
  gc1.addGeometry( part2.clone() );

  wkb = gc1.asWkb();
  QgsConstWkbPtr wkbPtr2( wkb );
  gc2.fromWkb( wkbPtr2 );

  QCOMPARE( gc2.numGeometries(), 2 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc2.geometryN( 0 ) ), part1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc2.geometryN( 1 ) ), part2 );


  //parts with m
  gc1.clear();
  gc2.clear();
  part1.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointM, 0, 0, 0, 1 )
                   << QgsPoint( QgsWkbTypes::PointM, 0, 10,  0, 2 )
                   << QgsPoint( QgsWkbTypes::PointM, 10, 10, 0, 3 )
                   << QgsPoint( QgsWkbTypes::PointM, 10, 0,  0, 4 )
                   << QgsPoint( QgsWkbTypes::PointM, 0, 0, 0, 1 ) );
  gc1.addGeometry( part1.clone() );
  part2.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointM, 1, 1, 0, 1 )
                   << QgsPoint( QgsWkbTypes::PointM, 1, 9, 0, 2 )
                   << QgsPoint( QgsWkbTypes::PointM, 9, 9, 0, 3 )
                   << QgsPoint( QgsWkbTypes::PointM, 9, 1, 0, 4 )
                   << QgsPoint( QgsWkbTypes::PointM, 1, 1, 0, 1 ) );
  gc1.addGeometry( part2.clone() );

  wkb = gc1.asWkb();
  QgsConstWkbPtr wkbPtr3( wkb );
  gc2.fromWkb( wkbPtr3 );

  QCOMPARE( gc2.numGeometries(), 2 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc2.geometryN( 0 ) ), part1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc2.geometryN( 1 ) ), part2 );

  // parts with ZM
  gc1.clear();
  gc2.clear();

  part1.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 5 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 10, 2, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 10, 3, 7 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 0, 4, 8 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 9 ) );
  gc1.addGeometry( part1.clone() );
  part2.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 2 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 9, 2, 3 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 9, 3, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 1, 4, 4 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 7 ) );
  gc1.addGeometry( part2.clone() );

  wkb = gc1.asWkb();
  QgsConstWkbPtr wkbPtr4( wkb );
  gc2.fromWkb( wkbPtr4 );

  QCOMPARE( gc2.numGeometries(), 2 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc2.geometryN( 0 ) ), part1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc2.geometryN( 1 ) ), part2 );


  //bad WKB - check for no crash
  gc2.clear();

  QgsConstWkbPtr nullPtr( nullptr, 0 );

  QVERIFY( !gc2.fromWkb( nullPtr ) );
  QCOMPARE( gc2.wkbType(), QgsWkbTypes::GeometryCollection );

  QgsPoint point( 1, 2 );
  QByteArray wkbPoint = point.asWkb();
  QgsConstWkbPtr wkbPointPtr( wkbPoint );

  QVERIFY( !gc2.fromWkb( wkbPointPtr ) );
  QCOMPARE( gc2.wkbType(), QgsWkbTypes::GeometryCollection );
}

void TestQgsGeometryCollection::toFromWKT()
{
  QgsGeometryCollection gc;
  QgsLineString part1;
  part1.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 5 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 10, 2, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 10, 3, 7 )
                   << QgsPoint( QgsWkbTypes::PointZM, 10, 0, 4, 8 )
                   << QgsPoint( QgsWkbTypes::PointZM, 0, 0, 1, 9 ) );
  gc.addGeometry( part1.clone() );

  QgsLineString part2;
  part2.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 2 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 9, 2, 3 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 9, 3, 6 )
                   << QgsPoint( QgsWkbTypes::PointZM, 9, 1, 4, 4 )
                   << QgsPoint( QgsWkbTypes::PointZM, 1, 1, 1, 7 ) );
  gc.addGeometry( part2.clone() );

  QString wkt = gc.asWkt();

  QVERIFY( !wkt.isEmpty() );

  gc.clear();

  QVERIFY( gc.fromWkt( wkt ) );
  QCOMPARE( gc.numGeometries(), 2 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 0 ) ), part1 );
  QCOMPARE( *static_cast< const QgsLineString * >( gc.geometryN( 1 ) ), part2 );

  //bad WKT
  gc.clear();

  QVERIFY( !gc.fromWkt( "Point()" ) );
  QVERIFY( gc.isEmpty() );
  QCOMPARE( gc.numGeometries(), 0 );
  QCOMPARE( gc.wkbType(), QgsWkbTypes::GeometryCollection );
}

void TestQgsGeometryCollection::exportImport()
{
  //as JSON
  QgsGeometryCollection exportC;

  QgsLineString part;
  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::Point, 0, 0 )
                  << QgsPoint( QgsWkbTypes::Point, 0, 10 )
                  << QgsPoint( QgsWkbTypes::Point, 10, 10 )
                  << QgsPoint( QgsWkbTypes::Point, 10, 0 )
                  << QgsPoint( QgsWkbTypes::Point, 0, 0 ) );
  exportC.addGeometry( part.clone() );

  // GML document for compare
  QDomDocument doc( "gml" );

  // as GML2
  QString expectedSimpleGML2( QStringLiteral( "<MultiGeometry xmlns=\"gml\"><geometryMember xmlns=\"gml\"><LineString xmlns=\"gml\"><coordinates xmlns=\"gml\" cs=\",\" ts=\" \">0,0 0,10 10,10 10,0 0,0</coordinates></LineString></geometryMember></MultiGeometry>" ) );
  QString res = elemToString( exportC.asGml2( doc ) );
  QGSCOMPAREGML( res, expectedSimpleGML2 );

  QString expectedGML2empty( QStringLiteral( "<MultiGeometry xmlns=\"gml\"/>" ) );
  QGSCOMPAREGML( elemToString( QgsGeometryCollection().asGml2( doc ) ), expectedGML2empty );

  //as GML3
  QString expectedSimpleGML3( QStringLiteral( "<MultiGeometry xmlns=\"gml\"><geometryMember xmlns=\"gml\"><LineString xmlns=\"gml\"><posList xmlns=\"gml\" srsDimension=\"2\">0 0 0 10 10 10 10 0 0 0</posList></LineString></geometryMember></MultiGeometry>" ) );
  res = elemToString( exportC.asGml3( doc ) );
  QCOMPARE( res, expectedSimpleGML3 );

  QString expectedGML3empty( QStringLiteral( "<MultiGeometry xmlns=\"gml\"/>" ) );
  QGSCOMPAREGML( elemToString( QgsGeometryCollection().asGml3( doc ) ), expectedGML3empty );

  // as JSON
  QString expectedSimpleJson( "{\"geometries\":[{\"coordinates\":[[0.0,0.0],[0.0,10.0],[10.0,10.0],[10.0,0.0],[0.0,0.0]],\"type\":\"LineString\"}],\"type\":\"GeometryCollection\"}" );
  res = exportC.asJson();
  QCOMPARE( res, expectedSimpleJson );

  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::Point, 1, 1 )
                  << QgsPoint( QgsWkbTypes::Point, 1, 9 )
                  << QgsPoint( QgsWkbTypes::Point, 9, 9 )
                  << QgsPoint( QgsWkbTypes::Point, 9, 1 )
                  << QgsPoint( QgsWkbTypes::Point, 1, 1 ) );
  exportC.addGeometry( part.clone() );

  QString expectedJson( "{\"geometries\":[{\"coordinates\":[[0.0,0.0],[0.0,10.0],[10.0,10.0],[10.0,0.0],[0.0,0.0]],\"type\":\"LineString\"},{\"coordinates\":[[1.0,1.0],[1.0,9.0],[9.0,9.0],[9.0,1.0],[1.0,1.0]],\"type\":\"LineString\"}],\"type\":\"GeometryCollection\"}" );
  res = exportC.asJson();
  QCOMPARE( res, expectedJson );

  QgsGeometryCollection exportFloat;
  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::Point, 10 / 9.0, 10 / 9.0 )
                  << QgsPoint( QgsWkbTypes::Point, 10 / 9.0, 100 / 9.0 )
                  << QgsPoint( QgsWkbTypes::Point, 100 / 9.0, 100 / 9.0 )
                  << QgsPoint( QgsWkbTypes::Point, 100 / 9.0, 10 / 9.0 )
                  << QgsPoint( QgsWkbTypes::Point, 10 / 9.0, 10 / 9.0 ) );
  exportFloat.addGeometry( part.clone() );

  part.setPoints( QgsPointSequence() << QgsPoint( QgsWkbTypes::Point, 2 / 3.0, 2 / 3.0 )
                  << QgsPoint( QgsWkbTypes::Point, 2 / 3.0, 4 / 3.0 )
                  << QgsPoint( QgsWkbTypes::Point, 4 / 3.0, 4 / 3.0 )
                  << QgsPoint( QgsWkbTypes::Point, 4 / 3.0, 2 / 3.0 )
                  << QgsPoint( QgsWkbTypes::Point, 2 / 3.0, 2 / 3.0 ) );
  exportFloat.addGeometry( part.clone() );

  QString expectedJsonPrec3( "{\"geometries\":[{\"coordinates\":[[1.111,1.111],[1.111,11.111],[11.111,11.111],[11.111,1.111],[1.111,1.111]],\"type\":\"LineString\"},{\"coordinates\":[[0.667,0.667],[0.667,1.333],[1.333,1.333],[1.333,0.667],[0.667,0.667]],\"type\":\"LineString\"}],\"type\":\"GeometryCollection\"}" );
  res = exportFloat.asJson( 3 );
  QCOMPARE( res, expectedJsonPrec3 );

  // as GML2
  QString expectedGML2( QStringLiteral( "<MultiGeometry xmlns=\"gml\"><geometryMember xmlns=\"gml\"><LineString xmlns=\"gml\"><coordinates xmlns=\"gml\" cs=\",\" ts=\" \">0,0 0,10 10,10 10,0 0,0</coordinates></LineString></geometryMember><geometryMember xmlns=\"gml\"><LineString xmlns=\"gml\"><coordinates xmlns=\"gml\" cs=\",\" ts=\" \">1,1 1,9 9,9 9,1 1,1</coordinates></LineString></geometryMember></MultiGeometry>" ) );
  res = elemToString( exportC.asGml2( doc ) );
  QGSCOMPAREGML( res, expectedGML2 );

  QString expectedGML2prec3( QStringLiteral( "<MultiGeometry xmlns=\"gml\"><geometryMember xmlns=\"gml\"><LineString xmlns=\"gml\"><coordinates xmlns=\"gml\" cs=\",\" ts=\" \">1.111,1.111 1.111,11.111 11.111,11.111 11.111,1.111 1.111,1.111</coordinates></LineString></geometryMember><geometryMember xmlns=\"gml\"><LineString xmlns=\"gml\"><coordinates xmlns=\"gml\" cs=\",\" ts=\" \">0.667,0.667 0.667,1.333 1.333,1.333 1.333,0.667 0.667,0.667</coordinates></LineString></geometryMember></MultiGeometry>" ) );
  res = elemToString( exportFloat.asGml2( doc, 3 ) );
  QGSCOMPAREGML( res, expectedGML2prec3 );

  //as GML3
  QString expectedGML3( QStringLiteral( "<MultiGeometry xmlns=\"gml\"><geometryMember xmlns=\"gml\"><LineString xmlns=\"gml\"><posList xmlns=\"gml\" srsDimension=\"2\">0 0 0 10 10 10 10 0 0 0</posList></LineString></geometryMember><geometryMember xmlns=\"gml\"><LineString xmlns=\"gml\"><posList xmlns=\"gml\" srsDimension=\"2\">1 1 1 9 9 9 9 1 1 1</posList></LineString></geometryMember></MultiGeometry>" ) );
  res = elemToString( exportC.asGml3( doc ) );
  QCOMPARE( res, expectedGML3 );

  QString expectedGML3prec3( QStringLiteral( "<MultiGeometry xmlns=\"gml\"><geometryMember xmlns=\"gml\"><LineString xmlns=\"gml\"><posList xmlns=\"gml\" srsDimension=\"2\">1.111 1.111 1.111 11.111 11.111 11.111 11.111 1.111 1.111 1.111</posList></LineString></geometryMember><geometryMember xmlns=\"gml\"><LineString xmlns=\"gml\"><posList xmlns=\"gml\" srsDimension=\"2\">0.667 0.667 0.667 1.333 1.333 1.333 1.333 0.667 0.667 0.667</posList></LineString></geometryMember></MultiGeometry>" ) );
  res = elemToString( exportFloat.asGml3( doc, 3 ) );
  QCOMPARE( res, expectedGML3prec3 );

  //asKML
  QString expectedKml( QStringLiteral( "<MultiGeometry><LinearRing><altitudeMode>clampToGround</altitudeMode><coordinates>0,0,0 0,10,0 10,10,0 10,0,0 0,0,0</coordinates></LinearRing><LinearRing><altitudeMode>clampToGround</altitudeMode><coordinates>1,1,0 1,9,0 9,9,0 9,1,0 1,1,0</coordinates></LinearRing></MultiGeometry>" ) );
  QCOMPARE( exportC.asKml(), expectedKml );

  QString expectedKmlPrec3( QStringLiteral( "<MultiGeometry><LinearRing><altitudeMode>clampToGround</altitudeMode><coordinates>1.111,1.111,0 1.111,11.111,0 11.111,11.111,0 11.111,1.111,0 1.111,1.111,0</coordinates></LinearRing><LinearRing><altitudeMode>clampToGround</altitudeMode><coordinates>0.667,0.667,0 0.667,1.333,0 1.333,1.333,0 1.333,0.667,0 0.667,0.667,0</coordinates></LinearRing></MultiGeometry>" ) );
  QCOMPARE( exportFloat.asKml( 3 ), expectedKmlPrec3 );
}


QGSTEST_MAIN( TestQgsGeometryCollection )
#include "testqgsgeometrycollection.moc"
