/*
* Copyright (C) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

import {AsyncCallback, Callback} from "./basic";

/**
 * Provides filesystem statistics APIs
 *
 * @since 8
 * @syscap SystemCapability.FileManagement.StorageService.SpatialStatistics
 */
declare namespace storageStatistics {
  /**
   * Get the totalSize of volume.
   *
   * @since 8
   */
  function getTotalSizeOfVolume(volumeUuid: string, callback: AsyncCallback<number>): void;
  function getTotalSizeOfVolume(volumeUuid: string): Promise<number>;
  /**
   * Get the free size Of volume.
   * 
   * @since 8
   */
  function getFreeSizeOfVolume(volumeUuid: string, callback: AsyncCallback<number>): void;
  function getFreeSizeOfVolume(volumeUuid: string): Promise<number>;

 /**
   * Get the bundlestat 
   * 
   * @since 8
   */

 export interface BundleStats {
  appSize: number;
  cacheSize: number;
  dataSize: number;
}
  function getBundleStats(volumeUuid: string, packageName: string,  callback: AsyncCallback<BundleStats>): void;
  function getBundleStats(volumeUuid: string, packageName: string): Promise<BundleStats>;

}

export default storageStatistics;