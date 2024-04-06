#!/bin/bash
cd src/gui/frontend
npm run build
cd ../..
rm -r gui/backend/static
mkdir gui/backend/static
cp -r gui/frontend/dist/css gui/backend/static
cp -r gui/frontend/dist/js gui/backend/static
cp -r gui/frontend/dist/favicon.ico gui/backend/favicon.ico
cd ..

