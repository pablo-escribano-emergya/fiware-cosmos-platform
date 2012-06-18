package es.tid.cosmos.mobility;

import java.io.FileInputStream;
import java.io.InputStream;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.cosmos.base.mapreduce.CosmosWorkflow;
import es.tid.cosmos.base.mapreduce.WorkflowList;
import es.tid.cosmos.base.util.ArgumentParser;
import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.mobility.activitydensity.ActivityDensityRunner;
import es.tid.cosmos.mobility.activitydensity.profile.ActivityDensityProfileRunner;
import es.tid.cosmos.mobility.adjacentextraction.AdjacentExtractionRunner;
import es.tid.cosmos.mobility.aggregatedmatrix.group.AggregatedMatrixGroupRunner;
import es.tid.cosmos.mobility.aggregatedmatrix.simple.AggregatedMatrixSimpleRunner;
import es.tid.cosmos.mobility.itineraries.ItinerariesRunner;
import es.tid.cosmos.mobility.labelling.bts.BtsLabellingRunner;
import es.tid.cosmos.mobility.labelling.client.ClientLabellingRunner;
import es.tid.cosmos.mobility.labelling.clientbts.ClientBtsLabellingRunner;
import es.tid.cosmos.mobility.labelling.join.LabelJoiningRunner;
import es.tid.cosmos.mobility.labelling.secondhomes.DetectSecondHomesRunner;
import es.tid.cosmos.mobility.mivs.MivsRunner;
import es.tid.cosmos.mobility.outpois.OutPoisRunner;
import es.tid.cosmos.mobility.parsing.ParsingRunner;
import es.tid.cosmos.mobility.pois.PoisRunner;
import es.tid.cosmos.mobility.populationdensity.PopulationDensityRunner;
import es.tid.cosmos.mobility.populationdensity.profile.PopulationDensityProfileRunner;
import es.tid.cosmos.mobility.preparing.PreparingRunner;

/**
 *
 * @author dmicol
 */
public class MobilityMain extends Configured implements Tool {
    @Override
    public int run(String[] args) throws Exception {
        WorkflowList wfList = new WorkflowList(); 
        ArgumentParser arguments = new ArgumentParser();
        arguments.parse(args);
        
        InputStream configInput;
        if (arguments.has("config")) {
            configInput = new FileInputStream(arguments.getString("config"));
        } else {
            configInput = Config.class.getResource("/mobility.properties")
                    .openStream();
        }
        final Configuration conf = Config.load(configInput, this.getConf());
        
        Path outputPath = new Path(arguments.getString("output", true));
        Path cdrsPath = new Path(arguments.getString("cdrs", true));
        Path cellsPath = new Path(arguments.getString("cells", true));
        Path cellGroupsPath = new Path(arguments.getString("cellGroups", true));
        Path adjBtsPath = new Path(arguments.getString("adjBts", true));
        Path btsVectorTxtPath = new Path(arguments.getString("btsVectorTxt",
                                                             true));
        Path clientProfilePath = new Path(arguments.getString("clientProfile",
                                                              true));
        
        boolean shouldRunAll = arguments.getBoolean("runAll");
        boolean isDebug = arguments.getBoolean("debug");
        
        Path tmpParsingPath = new Path(outputPath, "parsing");
        Path cdrsMobPath = new Path(tmpParsingPath, "cdrs_mob");
        Path cellsMobPath = new Path(tmpParsingPath, "cells_mob");
        Path pairbtsAdjPath = new Path(tmpParsingPath, "pairbts_adj");
        Path btsComareaPath = new Path(tmpParsingPath, "bts_comarea");
        Path clientProfileMobPath = new Path(tmpParsingPath,
                                             "clientprofile_mob");
        boolean shouldParse = arguments.getBoolean("parse");
        CosmosWorkflow parsingWorkflow = null;
        if (shouldRunAll || shouldParse) {
            parsingWorkflow = new ParsingRunner().run(cdrsPath, cdrsMobPath,
                    cellsPath, cellsMobPath, adjBtsPath, pairbtsAdjPath,
                    btsVectorTxtPath, btsComareaPath, clientProfilePath,
                    clientProfileMobPath, conf);
            wfList.add(parsingWorkflow);
        }
        
        Path tmpPreparingPath = new Path(outputPath, "preparing");
        Path cdrsInfoPath = new Path(tmpPreparingPath, "cdrs_info");
        Path cdrsNoinfoPath = new Path(tmpPreparingPath, "cdrs_noinfo");
        Path clientsBtsPath = new Path(tmpPreparingPath, "clients_bts");
        Path btsCommsPath = new Path(tmpPreparingPath, "bts_comms");
        Path cdrsNoBtsPath = new Path(tmpPreparingPath, "cdrs_no_bts");
        Path viTelmonthBtsPath = new Path(tmpPreparingPath, "vi_telmonth_bts");
        boolean shouldPrepare = arguments.getBoolean("prepare");
        CosmosWorkflow preparingWorkflow = null;
        if (shouldRunAll || shouldPrepare) {
            preparingWorkflow = new PreparingRunner().run(tmpPreparingPath,
                    cdrsMobPath, cdrsInfoPath, cdrsNoinfoPath, cellsPath,
                    clientsBtsPath, btsCommsPath, cdrsNoBtsPath,
                    viTelmonthBtsPath, isDebug, conf);
            preparingWorkflow.addDependentWorkflow(parsingWorkflow);
            wfList.add(preparingWorkflow);
        }

        Path tmpExtractMivsPath = new Path(outputPath, "mivs");
        Path viClientFuseAccPath = new Path(tmpExtractMivsPath,
                                            "vi_client_fuse_acc");
        boolean shouldExtractMivs = arguments.getBoolean("extractMIVs");
        CosmosWorkflow mivsWorkflow = null;
        if (shouldRunAll || shouldExtractMivs) {
            mivsWorkflow = new MivsRunner().run(viTelmonthBtsPath,
                    viClientFuseAccPath, tmpExtractMivsPath, isDebug, conf);
            mivsWorkflow.addDependentWorkflow(preparingWorkflow);
            wfList.add(mivsWorkflow);
        }
        
        Path tmpExtractPoisPath = new Path(outputPath, "pois");
        Path clientsInfoPath = new Path(tmpExtractPoisPath, "clients_info");
        Path clientsInfoFilteredPath = new Path(tmpExtractPoisPath,
                                                "clients_info_filtered");
        Path clientsRepbtsPath = new Path(tmpExtractPoisPath, "clients_repbts");
        boolean shouldExtractPois = arguments.getBoolean("extractPOIs");
        CosmosWorkflow poisWorkflow = null;
        if (shouldRunAll || shouldExtractPois) {
            poisWorkflow = new PoisRunner().run(tmpExtractPoisPath, clientsBtsPath,
                    clientsInfoPath, cdrsNoinfoPath, cdrsNoBtsPath,
                    clientsInfoFilteredPath, clientsRepbtsPath, isDebug, conf);
            poisWorkflow.addDependentWorkflow(preparingWorkflow);
            wfList.add(preparingWorkflow);
        }

        Path tmpLabelClientPath = new Path(outputPath, "label_client");
        Path vectorClientClusterPath = new Path(tmpLabelClientPath,
                                                "vector_client_cluster");
        boolean shouldLabelClient = arguments.getBoolean("labelClient");
        CosmosWorkflow clientLabellingWorkflow = null;
        if (shouldRunAll || shouldLabelClient) {
            Path centroidsPath = new Path(arguments.getString(
                    "centroids_client", true));
            clientLabellingWorkflow = new ClientLabellingRunner().run(cdrsMobPath,
                    clientsInfoFilteredPath, centroidsPath,
                    vectorClientClusterPath, tmpLabelClientPath, isDebug, conf);
            clientLabellingWorkflow.addDependentWorkflow(parsingWorkflow);
            clientLabellingWorkflow.addDependentWorkflow(poisWorkflow);
            wfList.add(clientLabellingWorkflow);
        }

        Path tmpLabelBtsPath = new Path(outputPath, "label_bts");
        Path vectorBtsClusterPath = new Path(tmpLabelBtsPath,
                                             "vector_bts_cluster");
        boolean shouldLabelBts = arguments.getBoolean("labelBTS");
        CosmosWorkflow btsLabellingWorkflow = null;
        if (shouldRunAll || shouldLabelBts) {
            Path centroidsPath = new Path(arguments.getString(
                    "centroids_bts", true));
            btsLabellingWorkflow = new BtsLabellingRunner().run(btsCommsPath,
                    btsComareaPath, centroidsPath, vectorBtsClusterPath,
                    tmpLabelBtsPath, isDebug, conf);
            btsLabellingWorkflow.addDependentWorkflow(parsingWorkflow);
            btsLabellingWorkflow.addDependentWorkflow(preparingWorkflow);
            wfList.add(btsLabellingWorkflow);
        }

        Path tmpLabelClientbtsPath = new Path(outputPath, "label_clientbts");
        Path vectorClientbtsPath = new Path(tmpLabelClientbtsPath,
                                            "vector_clientbts");
        Path pointsOfInterestTempPath = new Path(tmpLabelClientbtsPath,
                                                 "points_of_interest_temp");
        Path vectorClientbtsClusterPath = new Path(tmpLabelClientbtsPath,
                                                   "vector_clientbts_cluster");
        boolean shouldLabelClientbts = arguments.getBoolean("labelClientBTS");
        CosmosWorkflow clientBtsLabellingWorkflow = null;
        if (shouldRunAll || shouldLabelClientbts) {
            Path centroidsPath = new Path(arguments.getString(
                    "centroids_clientbts", true));
            clientBtsLabellingWorkflow = new ClientBtsLabellingRunner().run(
                    clientsInfoPath, clientsRepbtsPath, vectorClientbtsPath,
                    centroidsPath, pointsOfInterestTempPath,
                    vectorClientbtsClusterPath, tmpLabelClientbtsPath, isDebug,
                    conf);
            clientBtsLabellingWorkflow.addDependentWorkflow(poisWorkflow);
            wfList.add(clientBtsLabellingWorkflow);
        }

        Path tmpLabelJoining = new Path(outputPath, "label_joining");
        Path pointsOfInterestTemp4Path = new Path(tmpLabelJoining,
                                                  "points_of_interest_temp4");
        boolean shouldJoinLabels = arguments.getBoolean("joinLabels");
        CosmosWorkflow labelJoiningWorkflow = null;
        if (shouldRunAll || shouldJoinLabels) {
            labelJoiningWorkflow = new LabelJoiningRunner().run(
                    pointsOfInterestTempPath, vectorClientClusterPath,
                    vectorClientbtsClusterPath, vectorBtsClusterPath,
                    pointsOfInterestTemp4Path, tmpLabelJoining, isDebug, conf);
            labelJoiningWorkflow.addDependentWorkflow(clientBtsLabellingWorkflow);
            labelJoiningWorkflow.addDependentWorkflow(clientLabellingWorkflow);
            labelJoiningWorkflow.addDependentWorkflow(btsLabellingWorkflow);
            wfList.add(labelJoiningWorkflow);
        }
               
        Path tmpSecondHomesPath = new Path(outputPath, "second_homes");
        Path pointsOfInterestPath = new Path(tmpSecondHomesPath,
                                             "points_of_interest");
        boolean shouldDetectSecondHomes = arguments.getBoolean(
                "detectSecondHomes");
        CosmosWorkflow detectSecondHomesWorkflow =  null;
        if (shouldRunAll || shouldDetectSecondHomes) {
            detectSecondHomesWorkflow = new DetectSecondHomesRunner().run(
                    cellsMobPath, pointsOfInterestTemp4Path, viClientFuseAccPath,
                    pairbtsAdjPath, pointsOfInterestPath, tmpSecondHomesPath,
                    isDebug, conf);
            detectSecondHomesWorkflow.addDependentWorkflow(parsingWorkflow);
            detectSecondHomesWorkflow.addDependentWorkflow(labelJoiningWorkflow);
            detectSecondHomesWorkflow.addDependentWorkflow(mivsWorkflow);
            wfList.add(detectSecondHomesWorkflow);
        }
        
        Path tmpActivityDensityPath = new Path(outputPath, "activity_density");
        Path activityDensityOutPath = new Path(tmpActivityDensityPath,
                                               "activityDensityOut");
        boolean shouldGetActivityDensity = arguments.getBoolean(
                "getActivityDensity");
        CosmosWorkflow activityDensityWorkflow = null;
        if (shouldRunAll || shouldGetActivityDensity) {
            activityDensityWorkflow = new ActivityDensityRunner().run(
                    clientsInfoPath, activityDensityOutPath,
                    tmpActivityDensityPath, isDebug, conf);
            activityDensityWorkflow.addDependentWorkflow(poisWorkflow);
            wfList.add(activityDensityWorkflow);
        }
        
        Path tmpActivityDensityProfilePath = new Path(outputPath,
                "activity_density_profile");
        Path activityDensityProfileOutPath = new Path(
                tmpActivityDensityProfilePath, "activityDensityProfileOut");
        boolean shouldGetActivityDensityProfile = arguments.getBoolean(
                "getActivityDensityProfile");
        CosmosWorkflow activityDensityProfileWorkflow = null;
        if (shouldRunAll || shouldGetActivityDensityProfile) {
            activityDensityProfileWorkflow = new ActivityDensityProfileRunner()
                    .run(clientProfileMobPath, clientsInfoPath,
                         activityDensityProfileOutPath,
                         tmpActivityDensityProfilePath, isDebug, conf);
            activityDensityProfileWorkflow.addDependentWorkflow(parsingWorkflow);
            activityDensityProfileWorkflow.addDependentWorkflow(poisWorkflow);
            wfList.add(activityDensityProfileWorkflow);
        }
        
        Path tmpPopulationDensityPath = new Path(outputPath,
                                                 "population_density");
        Path populationDensityOutPath = new Path(tmpPopulationDensityPath,
                                               "populationDensityOut");
        boolean shouldGetPopulationDensity = arguments.getBoolean(
                "getPopulationDensity");
        CosmosWorkflow populationDensityWorkflow = null;
        if (shouldRunAll || shouldGetPopulationDensity) {
            populationDensityWorkflow = new PopulationDensityRunner().run(
                    cdrsInfoPath, cellsPath, populationDensityOutPath,
                    tmpPopulationDensityPath, isDebug, conf);
            populationDensityWorkflow.addDependentWorkflow(preparingWorkflow);
            populationDensityWorkflow.addDependentWorkflow(parsingWorkflow);
            wfList.add(populationDensityWorkflow);
        }

        Path tmpPopulationDensityProfilePath = new Path(outputPath,
                "population_density_profile");
        Path populationDensityProfileOutPath = new Path(
                tmpPopulationDensityProfilePath, "populationDensityProfileOut");
        boolean shouldGetPopulationDensityProfile = arguments.getBoolean(
                "getPopulationDensityProfile");
        CosmosWorkflow populationDensityProfileWorkflow = null;
        if (shouldRunAll || shouldGetPopulationDensityProfile) {
            populationDensityProfileWorkflow = new PopulationDensityProfileRunner()
                    .run(cdrsInfoPath, cellsPath, clientProfileMobPath,
                         populationDensityProfileOutPath,
                         tmpPopulationDensityProfilePath, isDebug, conf);
            populationDensityProfileWorkflow.addDependentWorkflow(preparingWorkflow);
            populationDensityProfileWorkflow.addDependentWorkflow(parsingWorkflow);
            wfList.add(populationDensityProfileWorkflow);
        }
        
        Path tmpAggregatedMatrixSimplePath = new Path(outputPath,
                "aggregated_matrix_simple");
        Path matrixPairBtsTxtPath = new Path(tmpAggregatedMatrixSimplePath,
                                             "matrixPairBtsTxt");
        boolean shouldGetAggregatedMatrixSimpleProfile = arguments.getBoolean(
                "getAggregatedMatrixSimple");
        CosmosWorkflow aggregatedMatrixSimpleWorkflow = null;
        if (shouldRunAll || shouldGetAggregatedMatrixSimpleProfile) {
            aggregatedMatrixSimpleWorkflow = new AggregatedMatrixSimpleRunner()
                    .run(cdrsInfoPath, cellsPath, matrixPairBtsTxtPath,
                         tmpAggregatedMatrixSimplePath, isDebug, conf);
            aggregatedMatrixSimpleWorkflow.addDependentWorkflow(preparingWorkflow);
            aggregatedMatrixSimpleWorkflow.addDependentWorkflow(parsingWorkflow);
            wfList.add(aggregatedMatrixSimpleWorkflow);
        }
        
        Path tmpAggregatedMatrixGroupPath = new Path(outputPath,
                                                     "aggregated_matrix_group");
        Path matrixPairGroupTxtPath = new Path(tmpAggregatedMatrixGroupPath,
                                               "matrixPairGroupTxt");
        boolean shouldGetAggregatedMatrixGroupProfile = arguments.getBoolean(
                "getAggregatedMatrixGroup");
        CosmosWorkflow aggregatedMatrixGroupWorkflow = null;
        if (shouldRunAll || shouldGetAggregatedMatrixGroupProfile) {
            aggregatedMatrixGroupWorkflow = new AggregatedMatrixGroupRunner()
                    .run(cdrsInfoPath, cellGroupsPath, matrixPairGroupTxtPath,
                         tmpAggregatedMatrixGroupPath, isDebug, conf);
            aggregatedMatrixGroupWorkflow.addDependentWorkflow(preparingWorkflow);
            aggregatedMatrixGroupWorkflow.addDependentWorkflow(parsingWorkflow);
            wfList.add(aggregatedMatrixGroupWorkflow);
        }
        
        wfList.waitForCompletion(true);
        
        // The following phases aren't modelled through CosmosWorkflows
        // because AdjancentExtractionRunner contains loops, which is currently
        // not supported by CosmosWorkflows
        Path tmpAdjacentsPath = new Path(outputPath, "adjacents");
        Path pointsOfInterestIdPath = new Path(tmpAdjacentsPath,
                                               "points_of_interest_id");
        boolean shouldExtractAdjacents = arguments.getBoolean(
                "extractAdjacents");
        if (shouldRunAll || shouldExtractAdjacents) {
            new AdjacentExtractionRunner().run(pointsOfInterestPath,
                    pairbtsAdjPath, pointsOfInterestIdPath, tmpAdjacentsPath,
                    isDebug, conf);
        }
        
        Path tmpItinerariesPath = new Path(outputPath, "itineraries");
        Path clientItinerariesTxtPath = new Path(tmpItinerariesPath,
                                                 "client_itineraries_txt");
        boolean shouldGetItineraries = arguments.getBoolean("getItineraries");
        if (shouldRunAll || shouldGetItineraries) {
            new ItinerariesRunner().run(cellsPath, cdrsInfoPath,
                                        pointsOfInterestIdPath,
                                        clientItinerariesTxtPath,
                                        tmpItinerariesPath, isDebug, conf);
        }
        
        Path tmpOutPoisPath = new Path(outputPath, "out_pois");
        boolean shouldOutPois = arguments.getBoolean("outPois");
        if (shouldRunAll || shouldOutPois) {
            new OutPoisRunner().run(vectorClientbtsPath, pointsOfInterestIdPath,
                                    vectorClientClusterPath, vectorBtsClusterPath,
                                    tmpOutPoisPath, isDebug, conf);
        }
        
        return 0;
    }
    
    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(),
                                     new MobilityMain(), args);
            if (res != 0) {
                throw new Exception("Unknown error");
            }
        } catch (Exception ex) {
            Logger.get(MobilityMain.class).fatal(ex);
            throw ex;
        }
    }
}
